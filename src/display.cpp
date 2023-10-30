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
    window = std::make_unique<sf::RenderWindow>(sf::VideoMode(properties.width, properties.height), properties.title);
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
        for(auto iterator = spriteList.begin(); iterator != spriteList.end(); iterator++) {
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
        spriteList.push_back(sprite);
    }
    return sprite;
}
