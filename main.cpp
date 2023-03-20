#include <iostream>
#include <sstream>
#include <iomanip>
#include <math.h>

#include <Eigen/Dense>
#include <SFML/Window.hpp>
#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>
#include <SFML/OpenGL.hpp>

const int WIDTH = 800;
const int HEIGHT = 400;

const float xSens = 0.01;
const float ySens = 0.01;

void mouseMove();

int main(){
    //fps stuff
    int frame_count = 0;
    int curr_fps = 60;
    float frame_time;
    float current_time;

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

    sf::Vector3f Origin = sf::Vector3f (0.f, 0.f, -0.3f);
    ShaderFrag.setUniform("origin", Origin);
    float speedMult = 2.f;

    //Mouse movement stuff
    sf::Glsl::Vec2 mouseDelta;

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

        //WASD movement
        if(sf::Keyboard::isKeyPressed(sf::Keyboard::A)){
            v.x -= frame_time * speedMult;
        }
        if(sf::Keyboard::isKeyPressed(sf::Keyboard::D)){
            v.x += frame_time * speedMult;
        }
        if(sf::Keyboard::isKeyPressed(sf::Keyboard::W)){
            v.z += frame_time * speedMult;
        }
        if(sf::Keyboard::isKeyPressed(sf::Keyboard::S)){
            v.z -= frame_time * speedMult;
        }
        Origin += v;
        v = sf::Vector3f (0.f, 0.f, 0.f);
        ShaderFrag.setUniform("origin", Origin);

        //Mouse rotate
//        sf::Vector2f center(resolution.x/2, resolution.y/2);
//        mouseDelta = sf::Glsl::Vec2 (sf::Vector2f (sf::Mouse::getPosition(window)) - center);
//        mouseDelta = sf::Vector2f (mouseDelta.x / resolution.x, mouseDelta.y / resolution.y);
//        sf::Mouse::setPosition(sf::Vector2i (center), window);
        if(sf::Event::MouseMoved){
            mouseDelta = window.mapPixelToCoords(sf::Mouse::getPosition(window));
        }
        ShaderFrag.setUniform("u_mouse_delta", sf::Glsl::Vec2(mouseDelta.x-1, mouseDelta.y-1));
        std::cout<<mouseDelta.x<<" "<<mouseDelta.y<< std::endl;



        //Enabling/disabling mouse cursor
        if(sf::Keyboard::isKeyPressed(sf::Keyboard::Escape)) {
            window.setMouseCursorVisible(true);
            window.setMouseCursorGrabbed(false);
        }
        if(sf::Mouse::isButtonPressed(sf::Mouse::Left)) {
            window.setMouseCursorVisible(false);
            window.setMouseCursorGrabbed(true);
        }


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
        sf::Font font;
        font.loadFromFile("/home/pavan/CLionProjects/RayMarcherEngine/Roboto-Black.ttf");
        sf::Text text(fps.str(), font, 30);
        text.setFillColor(sf::Color::Green);
        text.setPosition(37.f, 37.f);
        text.setOrigin(0,0);
        window.draw(text);


        window.display();
    }

    return 0;
}

void mouseMove(sf::Window win){
    sf::Vector2f center = sf::Vector2f(win.getSize());
    sf::Glsl::Vec2 mouseDelta = sf::Glsl::Vec2 (sf::Vector2f(sf::Mouse::getPosition(win))-center);
    sf::Mouse::setPosition(sf::Vector2i (center));

    sf::Glsl::Vec2 cos = sf::Glsl::Vec2 (std::cos(mouseDelta.x), std::cos(mouseDelta.y));
    sf::Glsl::Vec2 sin = sf::Glsl::Vec2 (std::sin(mouseDelta.x), std::sin(mouseDelta.y));

    float rot[9] = {
            cos.y, 0.f, -sin.y,
            sin.y * sin.x, cos.x, cos.y * sin.x,
            sin.y * cos.x, -sin.x, cos.y * cos.x
    };
    sf::Glsl::Mat3 rotMat(rot);


}


