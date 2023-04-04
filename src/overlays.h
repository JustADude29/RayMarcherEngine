//
// Created by pavan on 3/18/23.
//
#ifndef OVERLAYS_HPP
#define OVERLAYS_HPP

#include <iostream>
#include "SFML/Graphics.hpp"
namespace overlays {
    class button : public sf::Drawable{
        public:
            button();

            button(std::string text, sf::Font fonts, sf::Color col, sf::Vector2f pos, float Textsize, bool imageEnable);

            ~button();

            void setSprite(sf::Image image1, sf::Image image2);
            void setCol(sf::Color col);

            void update(sf::Event& e, sf::Window &window, sf::Color col1, sf::Color col2);

            bool activated;

    private:
            virtual void draw(sf::RenderTarget &target, sf::RenderStates states) const;

        private:
            std::string content;
            sf::Color baseCol;
            sf::Vector2f position;
            sf::Font font;
            float text_size;
            float borderThickness;
            sf::Text buttonText;

            bool sprite;
            sf::Texture t1;
            sf::Texture t2;

            sf::Vector2f size;
            sf::Sprite buttonImage;

            sf::RectangleShape button_shape;
        };
}
#endif

overlays::button::button(std::string text,sf::Font fonts, sf::Color col, sf::Vector2f pos, float Textsize, bool imageEnable= false) {
    content = text;
    font = fonts;
    baseCol = col;
    position = pos;
    text_size = Textsize;
    activated = false;


    borderThickness = 2;

    sprite = imageEnable;
    buttonText.setString(content);
    buttonText.setFont(font);
    buttonText.setCharacterSize(Textsize);
    buttonText.setFillColor(baseCol);

    buttonText.setOrigin(buttonText.getGlobalBounds().width/2, buttonText.getGlobalBounds().height/2);
    buttonText.setPosition(position);
    buttonImage.setOrigin(buttonImage.getGlobalBounds().width/2, buttonImage.getGlobalBounds().height/2);
    buttonImage.setPosition(position);

    size = sf::Vector2f (buttonText.getGlobalBounds().width*1.5, buttonText.getGlobalBounds().height*1.5);
    button_shape = sf::RectangleShape(size);
    button_shape.setOrigin(button_shape.getGlobalBounds().width/2, button_shape.getGlobalBounds().height/2);
    button_shape.setPosition(position);
}

overlays::button::~button() {

}

void overlays::button::setSprite(sf::Image image1, sf::Image image2) {
    t1.loadFromImage(image1);
    t2.loadFromImage(image2);
    buttonImage.setTexture(t1);
}

void overlays::button::setCol(sf::Color col){
    baseCol = col;
    buttonText.setFillColor(col);
}

void overlays::button::draw(sf::RenderTarget &target, sf::RenderStates states) const{
    if(sprite)
        target.draw(buttonImage);
    target.draw(buttonText);
}

void overlays::button::update(sf::Event& e, sf::Window &window, sf::Color col1, sf::Color col2) {
    sf::Vector2i mousePos = sf::Mouse::getPosition(window);

    bool mouseInButton = mousePos.x >=  button_shape.getPosition().x - button_shape.getGlobalBounds().width/2
                        && mousePos.x <= button_shape.getPosition().x + button_shape.getGlobalBounds().width/2
                        && mousePos.y >= button_shape.getPosition().y - button_shape.getGlobalBounds().height/2
                        && mousePos.y <= button_shape.getPosition().y + button_shape.getGlobalBounds().height/2;

    if(e.type == sf::Event::MouseMoved) {
        if (mouseInButton) {
            buttonText.setFillColor(col2);
            activated = true;
        } else {
            buttonText.setFillColor(col1);
            activated = false;
        }
    }
}