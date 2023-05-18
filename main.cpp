#include <iostream>
#include <sstream>
#include <iomanip>
#include <math.h>
#include "src/overlays.h"

#include <Eigen/Dense>
#include <SFML/Window.hpp>
#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>
#include <SFML/OpenGL.hpp>

//---------debug macros----------------
#define ping(x) std::cout<<"ping"<<x<<std::endl


const int WIDTH = 1600;
const int HEIGHT = 900;

const float xSens = 0.01;
const float ySens = 0.01;

Eigen::Matrix3f getCamRot(Eigen::Vector2f angle);

int main(){
    //fps stuff
    int frame_count = 0;
    int curr_fps = 60;
    float frame_time;
    float current_time;
    float q=0;
    int coun=0;
    int steps = 100;

    //create window
    sf::RenderWindow window;
    window.create(sf::VideoMode(WIDTH, HEIGHT), "RayMarching", sf::Style::Default);
    window.setFramerateLimit(curr_fps);
    window.setActive(true);

    //loading shader
    sf::Shader ShaderFrag;
    if(!ShaderFrag.loadFromFile("/home/dude/CLionProjects/RayMarcherEngine/rayMarch.frag", sf::Shader::Fragment)){
        std::cerr<<"Could not load Fragment Shader\n";
        return -1;
    }

    //set initial resolution
    sf::Glsl::Vec2 resolution(WIDTH, HEIGHT);
    ShaderFrag.setUniform("u_resolution", resolution);

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
    sf::Vector3f Origin = sf::Vector3f (0.f, 0.f, 0.f);
    ShaderFrag.setUniform("origin", Origin);
    float speedMult;
    float normMult = 5.f;
    float sprintMult = 40.f;
    bool sprint = false;

    //Mouse movement stuff
    sf::Glsl::Vec2 mouseDelta = sf::Glsl::Vec2 (0,0);
    sf::Glsl::Vec2 mousePos;
    bool mouseEnabled = false;
    float mouseSens = 0.003;

    //overlays
    sf::Font font;
    font.loadFromFile("/home/dude/CLionProjects/RayMarcherEngine/Roboto-Black.ttf");
    overlays::button exit_button("Exit", font, sf::Color::White, sf::Vector2f(37, 80), 20);
    overlays::button coordX("", font, sf::Color::White, sf::Vector2f(0, 0), 5);
    overlays::button coordY("", font, sf::Color::White, sf::Vector2f(0, 0), 5);
    overlays::button coordZ("", font, sf::Color::White, sf::Vector2f(0, 0), 5);
    overlays::button test("test", font, sf::Color::White, sf::Vector2f(200,80), 30, true);
    sf::Image image1, image2;
    image1.loadFromFile("/home/dude/CLionProjects/RayMarcherEngine/Homework folder.png");
    image2.loadFromFile("/home/dude/CLionProjects/RayMarcherEngine/idk.png");
    test.setSprite(image1, image2);

    window.setMouseCursorVisible(false);
    window.setMouseCursorGrabbed(true);
    bool running= true;
    window.setKeyRepeatEnabled(false);
    while (running){
        window.clear();


        //resizing and closing
        sf::Event event;
        while (window.pollEvent(event)){
            if(event.type == sf::Event::Closed){
                running = false;
            }
            else if(event.type == sf::Event::Resized){
                //sf::Glsl::Vec2 resolutionTemp(event.size.width, event.size.height);
                resolution = sf::Glsl::Vec2(event.size.width, event.size.height);
                ShaderFrag.setUniform("u_resolution", resolution);
            }
        }

        frame_time = clock2.restart().asSeconds();
        q+=frame_time;
        ShaderFrag.setUniform("u_Time", q);

        Origin += v;
        v = sf::Vector3f (0.f, 0.f, 0.f);
        ShaderFrag.setUniform("origin", Origin);

        //Mouse rotate
        sf::Vector2i center = sf::Vector2i(window.getSize().x/2, window.getSize().y/2);
        if(!mouseEnabled){
            mousePos = window.mapPixelToCoords(sf::Mouse::getPosition(window));
            mouseDelta += mousePos - sf::Glsl::Vec2(center);
            sf::Mouse::setPosition(center, window);
        }
        ShaderFrag.setUniform("u_mouse_delta", mouseDelta);
        ShaderFrag.setUniform("u_mouse_sensitivity", mouseSens);

        //WASD movement
        Eigen::Vector2f angle = Eigen::Vector2f(mouseDelta.y-resolution.y * 0.5, mouseDelta.x-resolution.x * 0.5);
        if(sf::Keyboard::isKeyPressed(sf::Keyboard::LShift)){
            sprint = true;
        }else{
            sprint = false;
        }
        if(sprint)
            speedMult = sprintMult;
        else
            speedMult = normMult;
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
            if (sf::Mouse::isButtonPressed(sf::Mouse::Left)) {
                window.setMouseCursorVisible(false);
                window.setMouseCursorGrabbed(true);
                mouseEnabled = false;
            }
        }

        //changing raymarch steps
        if(sf::Keyboard::isKeyPressed(sf::Keyboard::Up)){
            steps+=1;
        }
        if(sf::Keyboard::isKeyPressed(sf::Keyboard::Down)){
            steps-=1;
        }
        ShaderFrag.setUniform("MAX_STEPS", steps);

        //draw the rectangle with frag shader as its color
        window.draw(sprite, &ShaderFrag);

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
        sf::Text text(fps.str(), font, 20);
        text.setFillColor(sf::Color::Green);
        text.setPosition(37.f, 37.f);
        text.setOrigin(0,0);
        window.draw(text);


        exit_button.update(event, window, sf::Color::White, sf::Color(25,23,20, 180), "STEPS:"+std::to_string(steps));
        test.update(event, window, sf::Color::White, sf::Color::White);
        window.draw(exit_button);
        window.draw(test);

        if(exit_button.activated){
            if(sf::Mouse::isButtonPressed(sf::Mouse::Left)){
                running = false;
            }
        }

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


