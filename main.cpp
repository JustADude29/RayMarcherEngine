#include <iostream>
#include <sstream>
#include <iomanip>
#include <math.h>

#include "overlays.h"

#include <Eigen/Dense>
#include <SFML/Window.hpp>
#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>
#include <SFML/OpenGL.hpp>

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
    float q = 0;

    //create window
    sf::RenderWindow window;
    window.create(sf::VideoMode(WIDTH, HEIGHT), "RayMarching", sf::Style::Default);
    window.setFramerateLimit(curr_fps);
    window.setActive(true);

    //loading shader
    sf::Shader ShaderFrag;
    if(!ShaderFrag.loadFromFile("/home/pavan/CLionProjects/RayMarcherEngine/rayMarch.frag", sf::Shader::Fragment)){
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
    float speedMult = 2.f;

    //Mouse movement stuff
    sf::Glsl::Vec2 mouseDelta = sf::Glsl::Vec2 (0,0);
    sf::Glsl::Vec2 mousePos;
    bool mouseEnabled = false;
    float mouseSens = 0.003;

    //button
    sf::Font font;
    font.loadFromFile("/home/pavan/CLionProjects/RayMarcherEngine/Roboto-Black.ttf");
    sf::Image arrowImg;
    arrowImg.loadFromFile("/home/pavan/CLionProjects/RayMarcherEngine/arrow.png");
    overlays::button exitButtion("exit", font, sf::Color::Green, sf::Vector2f(1400, 850), 20);
    exitButtion.setSprite(arrowImg, arrowImg);

    window.setMouseCursorVisible(false);
    window.setMouseCursorGrabbed(true);
    bool running= true;
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

        Origin += v;
        v = sf::Vector3f (0.f, 0.f, 0.f);
        ShaderFrag.setUniform("origin", Origin);

        //Mouse rotate
        sf::Vector2i center = sf::Vector2i(window.getSize().x/2, window.getSize().y/2);
        if(sf::Event::MouseMoved && !mouseEnabled){
            mousePos = window.mapPixelToCoords(sf::Mouse::getPosition(window));
            mouseDelta += mousePos - sf::Glsl::Vec2(center);
        }
        sf::Mouse::setPosition(center, window);
        ShaderFrag.setUniform("u_mouse_delta", mouseDelta);
        ShaderFrag.setUniform("u_mouse_sensitivity", mouseSens);
//        std::cout<<center.x<<" "<<center.y<< std::endl;
//        std::cout<<mouseDelta.x<<" "<<mouseDelta.y<< std::endl;
//        std::cout<<std::endl;

        //WASD movement
        Eigen::Vector2f angle = Eigen::Vector2f(mouseDelta.y-resolution.y * 0.5, mouseDelta.x-resolution.x * 0.5);
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
        if(sf::Keyboard::isKeyPressed(sf::Keyboard::LShift)){
            Origin -= sf::Glsl::Vec3 (up.x(), up.y(), up.z());
        }
        

        //Enabling/disabling mouse cursor
        if(sf::Keyboard::isKeyPressed(sf::Keyboard::Escape)) {
            window.setMouseCursorVisible(true);
            window.setMouseCursorGrabbed(false);
            mouseEnabled = true;
        }
        if(sf::Mouse::isButtonPressed(sf::Mouse::Left)) {
            window.setMouseCursorVisible(false);
            window.setMouseCursorGrabbed(true);
            mouseEnabled = false;
        }


        //draw the rectangle with frag shader as its color
        window.draw(sprite, &ShaderFrag);

        //get fps
        current_time = clock1.getElapsedTime().asSeconds();
        q += frame_time;
        ShaderFrag.setUniform("u_Time", q);

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


