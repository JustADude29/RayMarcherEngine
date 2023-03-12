#include <iostream>
#include <sstream>
#include <iomanip>

#include <SFML/Window.hpp>
#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>
#include <SFML/OpenGL.hpp>

const int WIDTH = 1600;
const int HEIGHT = 900;

int main(){
    int frame_count = 0;
    int curr_fps = 60;
    float frame_time;
    float current_time;

    sf::RenderWindow window;
    window.create(sf::VideoMode(WIDTH, HEIGHT), "RayMarching", sf::Style::Default);
    window.setFramerateLimit(curr_fps);
    window.setActive(true);

    sf::Shader ShaderFrag;
    ShaderFrag.loadFromFile("/home/pavan/CLionProjects/RayMarcherEngine/rayMarch.frag", sf::Shader::Fragment);
    sf::Glsl::Vec2 resolution(WIDTH, HEIGHT);
    ShaderFrag.setUniform("u_resolution", resolution);

    sf::Clock clock1;
    sf::Clock clock2;

    sf::Image image;
    image.create(resolution.x, resolution.y, sf::Color::Transparent);

    sf::Texture texture;
    texture.loadFromImage(image);

    sf::Sprite sprite;
    sprite.setTexture(texture);

    auto shape = sf::RectangleShape{sf::Vector2f{window.getSize()}};

    float q=0;

    sf::Vector3f v = sf::Vector3f (0.f,0.f,0.f);

    sf::Vector3f Origin = sf::Vector3f (0.f, 0.f, -0.3f);
    ShaderFrag.setUniform("origin", Origin);
    float speedMult = 2.f;

    bool running= true;
    while (running){
        window.clear();

        sf::Event event;
        while (window.pollEvent(event)){
            if(event.type == sf::Event::Closed){
                running = false;
            }
            else if(event.type == sf::Event::Resized){
                sf::Glsl::Vec2 resolutionTemp(event.size.width, event.size.height);
                ShaderFrag.setUniform("u_resolution", resolutionTemp);
            }
        }

        frame_time = clock2.restart().asSeconds();

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


        window.draw(sprite, &ShaderFrag);

        current_time = clock1.getElapsedTime().asSeconds();

        frame_count+=1;
        if(current_time>=1.0){
            curr_fps = frame_count;
            frame_count = 0;
            clock1.restart();
        }

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
