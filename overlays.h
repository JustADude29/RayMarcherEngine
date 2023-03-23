//
// Created by pavan on 3/18/23.
//
#pragma once

#include <SFML/Graphics.hpp>
namespace overlays {
    class button {
    public:
        button();

        button(std::string text, sf::Font fonts, sf::Color col, sf::Vector2f pos, float Textsize);

        ~button();

        void setSprite(sf::Image image1, sf::Image image2);

        void draw(sf::RenderTarget &target);

    private:
        std::string content;
        sf::Color baseCol;
        sf::Vector2f position;
        sf::Font font;
        float size;
        float borderThickness;
        sf::Text buttonText;

        sf::Texture t1;
        sf::Texture t2;
        sf::Sprite buttonImage;
    };
}

overlays::button::button(std::string text,sf::Font fonts, sf::Color col, sf::Vector2f pos, float Textsize) {
    content = text;
    font = fonts;
    baseCol = col;
    position = pos;
    size = Textsize;

    borderThickness = 2;

    buttonText.setString(content);
    buttonText.setFont(font);

    buttonImage.setPosition(position);
}

void overlays::button::setSprite(sf::Image image1, sf::Image image2) {
    t1.loadFromImage(image1);
    t2.loadFromImage(image2);
    buttonImage.setTexture(t1);
}

void overlays::button::draw(sf::RenderTarget &target) {
    target.draw(buttonImage);
    target.draw(buttonText);
}


