//
// Created by pavan on 3/18/23.
//
#pragma once

#include <SFML/Graphics.hpp>

class button{
public:
    button();
    button(std::string text, sf::Color col, sf::Vector2i pos);
    ~button();

    void setColor(sf::Color col){};


private:
    std::string content;
    sf::Color baseCol;
    sf::Vector2i position;
};

button::button(std::string text, sf::Color col, sf::Vector2i pos) {
    content = text;
    baseCol = col;
    position = pos;


}


