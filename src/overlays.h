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
            void setSpriteScale(sf::Vector2f factor);
            void setCol(sf::Color col);
            void setText(std::string newText);

            void update(sf::Event& e, sf::Window &window, sf::Color col1, sf::Color col2);
            void update(sf::Event& e, sf::Window &window, sf::Color col1, sf::Color col2, std::string text);

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

            bool isSprite;
            sf::Texture t1;
            sf::Texture t2;

            sf::Vector2f size;
            sf::Sprite buttonImage;

            sf::RectangleShape button_shape;

    };

    template <typename T>
    class slider{
        public:
            slider();

            slider(std::string text, sf::Font font, T lowerLimit, T upperLimit, T defaultValue);

            ~slider();

        private:
            button sliderButton;
            T sliderValue;
            T lowerLimit;
            T upperLimit;
            sf::Text sliderText;
            std::string sliderContent;
    };
}
#endif

//-------------------------------------------------------BUTTON FUNCTIONS START--------------------------------------------------->
overlays::button::button(std::string text,sf::Font fonts, sf::Color col, sf::Vector2f pos, float Textsize, bool imageEnable = false) {
    content = text;
    font = fonts;
    baseCol = col;
    position = pos;
    text_size = Textsize;
    activated = false;


    borderThickness = 2;

    isSprite = imageEnable;
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
    size = sf::Vector2f (buttonImage.getGlobalBounds().width*1.5, buttonImage.getGlobalBounds().height*1.5);
//    std::cout<<size.x<<" "<<size.y<<"hello"<<std::endl;
    button_shape = sf::RectangleShape(size);
    button_shape.setOrigin(button_shape.getGlobalBounds().width/2, button_shape.getGlobalBounds().height/2);
    button_shape.setPosition(position);
}

void overlays::button::setSpriteScale(sf::Vector2f factor){
    buttonImage.setScale(factor);
    setSprite(t1.copyToImage(), t2.copyToImage());
}

void overlays::button::setCol(sf::Color col){
    baseCol = col;
    buttonText.setFillColor(col);
}

void overlays::button::setText(std::string newText) {
    buttonText.setString(newText);
}

void overlays::button::draw(sf::RenderTarget &target, sf::RenderStates states) const{
    if(isSprite)
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

void overlays::button::update(sf::Event& e, sf::Window &window, sf::Color col1, sf::Color col2, std::string text) {
    sf::Vector2i mousePos = sf::Mouse::getPosition(window);

    bool mouseInButton = mousePos.x >=  button_shape.getPosition().x - button_shape.getGlobalBounds().width/2
                         && mousePos.x <= button_shape.getPosition().x + button_shape.getGlobalBounds().width/2
                         && mousePos.y >= button_shape.getPosition().y - button_shape.getGlobalBounds().height/2
                         && mousePos.y <= button_shape.getPosition().y + button_shape.getGlobalBounds().height/2;

    if(e.type == sf::Event::MouseMoved) {
        if (mouseInButton) {
            buttonText.setFillColor(col2);
            if(isSprite)
                buttonImage.setTexture(t2);
            activated = true;
        } else {
            buttonText.setFillColor(col1);
            if(isSprite)
                buttonImage.setTexture(t1);
            activated = false;
        }
    }

    buttonText.setString(text);
}

//-------------------------------------------------------BUTTON FUNCTIONS END--------------------------------------------------->


//-------------------------------------------------------SLIDER FUNCTIONS START--------------------------------------------------->
template <typename T>
overlays::slider<T>::slider(std::string text, sf::Font font, T lowerLimit, T upperLimit, T defaultValue){
    overlays::button sliderButton();
}



//-------------------------------------------------------SLIDER FUNCTIONS END----------------------------------------------------->