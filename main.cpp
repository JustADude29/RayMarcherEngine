#include <iostream>
#include <sstream>
#include <iomanip>
#include <math.h>
#include <filesystem>

#include <Eigen/Dense>
#include <SFML/Window.hpp>
#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>
#include <SFML/OpenGL.hpp>

#include "src/overlays.h"


//---------debug macros----------------
#define ping(x) std::cout<<"ping"<<x<<std::endl


const int WIDTH = 1600;
const int HEIGHT = 900;

const float xSens = 0.01;
const float ySens = 0.01;

Eigen::Matrix3f getCamRot(Eigen::Vector2f angle);
bool leftClickListen();

int main(int argv, char* argc[]){
    //fps stuff
    int frame_count = 0;
    int curr_fps = 60;
    float frame_time;
    float current_time;
    float q=0;
    int steps = 100;


//    std::string curr_path = std::filesystem::current_path();
//    curr_path+="/..";
    std::string curr_path = argc[0];
    while(curr_path.back()!='/') curr_path.pop_back();
//    curr_path+="/..";
    ping(curr_path);

    //create window
    sf::RenderWindow window;
    window.create(sf::VideoMode::getDesktopMode(), "RayMarching");
    window.setFramerateLimit(curr_fps);
    window.setActive(true);


    //loading shader
    sf::Shader ShaderFrag[5];
    if(!ShaderFrag[0].loadFromFile(curr_path + "rayMarch.frag", sf::Shader::Fragment)){
        std::cerr<<"Could not load Fragment Shader\n";
        return -1;
    }
    int currFrag = 0;


    //set initial resolution
    sf::Glsl::Vec2 resolution = sf::Glsl::Vec2 (window.getSize().x, window.getSize().y);
    ShaderFrag[currFrag].setUniform("u_resolution", resolution);


    sf::Clock clock1;
    sf::Clock clock2;


    //creating image(rectangle) to put the color(frag) on
    sf::Image image;
    image.create(resolution.x, resolution.y, sf::Color::Transparent);
    sf::Texture texture;
    texture.loadFromImage(image);
    sf::Sprite sprite;
    sprite.setTexture(texture);
    auto shape = sf::RectangleShape{sf::Vector2f{window.getSize()}};


    //WASD movement stuff
    sf::Vector3f v = sf::Vector3f (0.f,0.f,1.f);
    sf::Vector3f Origin = sf::Vector3f (10.f, 0.f, 0.f);
    ShaderFrag[currFrag].setUniform("origin", Origin);
    float speedMult;
    float normMult = 15.f;
    float sprintMult = 40.f;


    //Mouse movement stuff
    sf::Glsl::Vec2 mouseDelta = sf::Glsl::Vec2 (0,0);
    sf::Glsl::Vec2 mousePos;
    bool mouseEnabled = false;
    float mouseSens = 0.003;


    //overlays
    sf::Font font;
    font.loadFromFile(curr_path+"assets/Roboto-Black.ttf");
    overlays::button exit_button("EXIT", font, sf::Color::White, sf::Vector2f(7 * resolution.x/8, 9.5f * resolution.y/10), 40);

    sf::Image increaseImage;
    sf::Image increaseImageDark;
    overlays::button increaseSteps_button("", font, sf::Color::White, sf::Vector2f(7.5f * resolution.x/8, resolution.y/10), 10, true);
    if(increaseImage.loadFromFile(curr_path+"assets/plus.png") && increaseImageDark.loadFromFile(curr_path+"assets/plus_dark.png")){
        increaseSteps_button.setSprite(increaseImage, increaseImageDark);
        increaseSteps_button.setSpriteScale(sf::Vector2f(0.5,0.5));
    } else {
        increaseSteps_button.setText("Increase steps");
    }

    sf::Image decreaseImage;
    sf::Image decreaseImageDark;
    overlays::button decreaseSteps_button("", font, sf::Color::White, sf::Vector2f(6.5f * resolution.x/8, resolution.y/10), 10,true);
    if(decreaseImage.loadFromFile(curr_path+"assets/minus.png") && decreaseImageDark.loadFromFile(curr_path+"assets/minus_dark.png")) {
        decreaseSteps_button.setSprite(decreaseImage, decreaseImageDark);
        decreaseSteps_button.setSpriteScale(sf::Vector2f(0.5,0.5));
    } else {
        decreaseSteps_button.setText("Decrease Steps");
    }

    overlays::button stepsTextButton("1000", font, sf::Color::Black, sf::Vector2f(7 * resolution.x / 8, resolution.y / 10), 40);
    sf::Text stepsInstructionsText("Current number of Ray Marching Steps(click to reset)", font, 15);
    stepsInstructionsText.setFillColor(sf::Color(100,20,20,120));
    stepsInstructionsText.setPosition(sf::Vector2f(6.3f * resolution.x/8, 0.5f * resolution.y/10));

    sf::Color defCol1 = sf::Color::Black;
    overlays::button infiniterButton("Click to toggle infinite", font, defCol1, sf::Vector2f(7 * resolution.x / 8, 2 * resolution.y / 10), 20);

    sf::Color defCol2 = sf::Color::Black;
    overlays::button outlineButton("Click to toggle outline", font, defCol2, sf::Vector2f(7 * resolution.x / 8, 2.5 * resolution.y / 10), 20);


    //UI menu background
    sf::RectangleShape rect(sf::Vector2f(resolution.x/4-20.0f, resolution.y-10.0f));
    rect.setOrigin(rect.getGlobalBounds().width/2, rect.getGlobalBounds().height/2);
    rect.setPosition(sf::Vector2f(7*resolution.x/8, resolution.y/2));
    rect.setFillColor(sf::Color(220,220,220,100));
    rect.setOutlineColor(sf::Color::Black);
    rect.setOutlineThickness(2);


    // window settings
    window.setMouseCursorVisible(false);
    window.setMouseCursorGrabbed(true);
    bool running= true;
    window.setKeyRepeatEnabled(false);


    // misc fragment shader settings
    bool infinite = false;
    bool outline = false;
    int coolDownTimer = 0;


    while (running){
        window.clear();

        //resizing and closing
        sf::Event event;
        while (window.pollEvent(event)){
            if(event.type == sf::Event::Closed){
                running = false;
            }
            else if(event.type == sf::Event::Resized){
                resolution = sf::Glsl::Vec2(event.size.width, event.size.height);
                ShaderFrag[currFrag].setUniform("u_resolution", resolution);
            }
        }

        frame_time = clock2.restart().asSeconds();
        q+=frame_time;
        ShaderFrag[currFrag].setUniform("u_Time", q);

        Origin += v;
        v = sf::Vector3f (0.f, 0.f, 0.f);
        ShaderFrag[currFrag].setUniform("origin", Origin);

        //Mouse rotate
        sf::Vector2i center = sf::Vector2i(resolution.x/2, resolution.y/2);
        if(!mouseEnabled){
            mousePos = window.mapPixelToCoords(sf::Mouse::getPosition(window));
            mouseDelta += mousePos - sf::Glsl::Vec2(center);
            sf::Mouse::setPosition(center, window);
        }
        ShaderFrag[currFrag].setUniform("u_mouse_delta", mouseDelta);
        ShaderFrag[currFrag].setUniform("u_mouse_sensitivity", mouseSens);

        //WASD movement
        Eigen::Vector2f angle = Eigen::Vector2f(mouseDelta.y-resolution.y * 0.5, mouseDelta.x-resolution.x * 0.5);
        if(sf::Keyboard::isKeyPressed(sf::Keyboard::LShift)){
            speedMult=sprintMult;
        }else{
            speedMult=normMult;
        }
        Eigen::Vector3f forward = getCamRot(angle * mouseSens) * Eigen::Vector3f (0,0,0.01) * speedMult;
        Eigen::Vector3f right = getCamRot(angle * mouseSens) * Eigen::Vector3f (0.01,0,0) * speedMult;
        Eigen::Vector3f up = getCamRot(angle * mouseSens) * Eigen::Vector3f (0,0.01,0) * speedMult;
        if(sf::Keyboard::isKeyPressed(sf::Keyboard::W)){
            Origin += sf::Glsl::Vec3 (forward.x(), forward.y(), forward.z());
        }
        if(sf::Keyboard::isKeyPressed(sf::Keyboard::S)){
            Origin -= sf::Glsl::Vec3 (forward.x(), forward.y(), forward.z());
        }
        if(sf::Keyboard::isKeyPressed(sf::Keyboard::D)){
            Origin += sf::Glsl::Vec3 (right.x(), right.y(), right.z());
        }
        if(sf::Keyboard::isKeyPressed(sf::Keyboard::A)){
            Origin -= sf::Glsl::Vec3 (right.x(), right.y(), right.z());
        }
        if(sf::Keyboard::isKeyPressed(sf::Keyboard::Space)){
            Origin += sf::Glsl::Vec3 (up.x(), up.y(), up.z());
        }
        if(sf::Keyboard::isKeyPressed(sf::Keyboard::LControl)){
            Origin -= sf::Glsl::Vec3 (up.x(), up.y(), up.z());
        }


        //Enabling/disabling mouse cursor
        if(sf::Event::KeyReleased && !mouseEnabled) {
            if (event.key.code == sf::Keyboard::Escape) {
                window.setMouseCursorVisible(true);
                window.setMouseCursorGrabbed(false);
                mouseEnabled = true;
            }
        }
        if(mouseEnabled) {
            if (leftClickListen()) {
                if(window.mapPixelToCoords(sf::Mouse::getPosition(window)).x < 3 * resolution.x/4) {
                    sf::Mouse::setPosition(center, window);
                    window.setMouseCursorVisible(false);
                    window.setMouseCursorGrabbed(true);
                    mouseEnabled = false;
                }
            }
        }


        //draw the rectangle with frag shader as its color
        window.draw(sprite, &ShaderFrag[currFrag]);

        //get fps
        current_time = clock1.getElapsedTime().asSeconds();

        frame_count+=1;
        if(current_time>=1.0){
            curr_fps = frame_count;
            frame_count = 0;
            clock1.restart();
        }

        //fps display
        std::ostringstream fps;
        fps << "fps: " << curr_fps;
        sf::Text fps_text(fps.str(), font, 20);
        sf::Text steps_text("STEPS:"+std::to_string(steps), font, 20);
        fps_text.setFillColor(sf::Color::Green);
        fps_text.setPosition(37.f, 37.f);
        fps_text.setOrigin(0, 0);
        steps_text.setFillColor(sf::Color::Red);
        steps_text.setPosition(37.f, 57.f);
        steps_text.setOrigin(0, 0);
        window.draw(fps_text);
        window.draw(steps_text);

        if(mouseEnabled) {
            window.draw(rect);
            window.draw(exit_button);
            window.draw(increaseSteps_button);
            window.draw(decreaseSteps_button);
            window.draw(stepsTextButton);
            window.draw(stepsInstructionsText);
            window.draw(infiniterButton);
            window.draw(outlineButton);
        }

        exit_button.update(event, window, sf::Color::White, sf::Color(25,23,20, 180), "EXIT");
        increaseSteps_button.update(event, window, sf::Color::White, sf::Color::White, "");
        decreaseSteps_button.update(event, window, sf::Color::White, sf::Color::White, "");
        stepsTextButton.update(event, window, sf::Color(3,3,3, 255), sf::Color::Blue, std::to_string(steps));
        infiniterButton.update(event, window, defCol1, sf::Color(20, 20, 100, 120));
        outlineButton.update(event, window, defCol2, sf::Color(20, 20, 100, 120));

        if(exit_button.activated){
            if(leftClickListen()){
                running = false;
            }
        }

        if(increaseSteps_button.activated){
            if(leftClickListen()){
                steps++;
            }
        }
        if(decreaseSteps_button.activated){
            if(leftClickListen()){
                steps--;
                if(steps<=0) steps=0;
            }
        }
        ShaderFrag[currFrag].setUniform("u_steps", steps);

        if(stepsTextButton.activated){
            if(leftClickListen()){
                steps = 100;
            }
        }

        if(infiniterButton.activated){
            if(leftClickListen() && coolDownTimer == 0){
                if(!infinite){
                    infinite = true;
                    defCol1 = sf::Color(20, 80, 20, 255);
                    coolDownTimer = 60;
                } else {
                    infinite = false;
                    defCol1 = sf::Color::Black;
                    coolDownTimer = 60;
                }
            }
        }

        if(outlineButton.activated){
            if(leftClickListen() && coolDownTimer == 0){
                if(!outline){
                    outline = true;
                    defCol2 = sf::Color(20, 80, 20, 255);
                    coolDownTimer = 60;
                } else {
                    outline = false;
                    defCol2 = sf::Color::Black;
                    coolDownTimer = 60;
                }
            }
        }


        coolDownTimer--;
        if(coolDownTimer<=0) coolDownTimer = 0;

        ShaderFrag[currFrag].setUniform("u_infinite", infinite);
        ShaderFrag[currFrag].setUniform("u_outline", outline);

        window.display();
    }

    return 0;
}

Eigen::Matrix3f getCamRot(Eigen::Vector2f angle){
    Eigen::Vector2f cos = Eigen::Vector2f (std::cos(angle.x()), std::cos(angle.y()));
    Eigen::Vector2f sin = Eigen::Vector2f (std::sin(angle.x()), std::sin(angle.y()));

    float vals[9] = {cos.y(), 0, -1*sin.y(),
                     sin.y()*sin.x(), cos.x(), cos.y()*sin.x(),
                     sin.y()*cos.x(), -1*sin.x(), cos.y()*cos.x()};
    Eigen::Matrix3f mat(vals);
    return mat;
}

bool leftClickListen(){
    if(sf::Mouse::isButtonPressed(sf::Mouse::Left)) return true;
    return false;
}


