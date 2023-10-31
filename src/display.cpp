//
// Created by zeroc00l on 10/30/23.
//


#include "../include/display.hpp"

void DisplayManager::InitWindow(WindowProperties properties) {
    if(window) {
        if(window->isOpen()) {
            window->close();
        }
        window.release();
    }
    window = std::make_unique<sf::RenderWindow>(sf::VideoMode(properties.width, properties.height), properties.title );
    uiScaleX = 2.0f;
    uiScaleY = 2.0f;
}

void DisplayManager::CloseWindow() {
    if(window) {
        window->close();
    }
}

sf::RenderWindow* DisplayManager::GetWindow() const {
    return window.get();
}

void DisplayManager::Update() {
    if(window) {
        window->clear();
        for(auto iterator = spriteList.begin(); iterator != spriteList.end(); ++iterator) {
            window->draw(**iterator);
        }
        for(auto iterator = textList.begin(); iterator != textList.end(); ++iterator) {
            window->draw(**iterator);
        }
        window->display();
    }
}

sf::Sprite* DisplayManager::CreateSprite(sf::Texture* texture) {
    sf::Sprite* sprite(nullptr);
    if(texture) {
        sprite = new sf::Sprite();
        sprite->setTexture(*texture);
        sprite->setScale(uiScaleX, uiScaleY);
        spriteList.push_back(sprite);
    }
    return sprite;
}

sf::Text* DisplayManager::CreateText(sf::Font* font) {
    sf::Text* text(nullptr);
    if(font) {
        text = new sf::Text();
        text->setFont(*font);
        text->setScale(uiScaleX, uiScaleY);
        textList.push_back(text);
    }
    return text;
}

std::pair<float, float> DisplayManager::GetUIScale() const {
    return std::make_pair<float,float>((const float)uiScaleX, (const float)uiScaleY);
}
