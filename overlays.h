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

        private:
            virtual void draw(sf::RenderTarget &target, sf::RenderStates states) const;

        private:
            std::string content;
            sf::Color baseCol;
            sf::Vector2f position;
            sf::Font font;
            float size;
            float borderThickness;
            sf::Text buttonText;

            bool sprite;
            sf::Texture t1;
            sf::Texture t2;
            sf::Sprite buttonImage;
        };
}
#endif

overlays::button::button(std::string text,sf::Font fonts, sf::Color col, sf::Vector2f pos, float Textsize, bool imageEnable= false) {
    content = text;
    font = fonts;
    baseCol = col;
    position = pos;
    size = Textsize;

    borderThickness = 2;

    sprite = imageEnable;
    buttonText.setString(content);
    buttonText.setFont(font);
    buttonText.setCharacterSize(Textsize);

//    buttonText.setOrigin(buttonText.getGlobalBounds().width/2, buttonText.getGlobalBounds().height/2);
    buttonText.setOrigin(0,0);
    buttonText.setPosition(position);
//    buttonImage.setOrigin(buttonImage.getGlobalBounds().width/2, buttonImage.getGlobalBounds().height/2);
    buttonImage.setOrigin(0,0);
    buttonImage.setPosition(position);
}

overlays::button::~button() {

}

void overlays::button::setSprite(sf::Image image1, sf::Image image2) {
    t1.loadFromImage(image1);
    t2.loadFromImage(image2);
    buttonImage.setTexture(t1);
}

void overlays::button::draw(sf::RenderTarget &target, sf::RenderStates states) const{
    if(sprite)
        target.draw(buttonImage);
    target.draw(buttonText);
}


