//
// Created by zeroc00l on 10/30/23.
//


#include "../include/display.hpp"

DisplaySystem::DisplaySystem(const DisplayConfig& config):
    System(SystemID::Display),
    uiScaleX(config.uiScaleX), uiScaleY(config.uiScaleY), windowProperties(config.windowProperties) {

}

DisplaySystem::~DisplaySystem() {

}

void DisplaySystem::Init() {
    InitWindow(windowProperties);
}

void DisplaySystem::Update() {
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

void DisplaySystem::Shutdown() {
    if(window) {
        if(window->isOpen()) {
            window->close();
        }
        window.release();
        window = nullptr;
    }
}

void DisplaySystem::InitWindow(WindowProperties properties) {
    if(window) {
        if(window->isOpen()) {
            window->close();
        }
        window.release();
        window = nullptr;
    }
    window = std::make_unique<sf::RenderWindow>(sf::VideoMode(properties.width, properties.height), properties.title );
}

void DisplaySystem::CloseWindow() {
    if(window) {
        window->close();
    }
}

sf::RenderWindow* DisplaySystem::GetWindow() const {
    return window.get();
}

sf::Sprite* DisplaySystem::CreateSprite(sf::Texture* texture) {
    sf::Sprite* sprite(nullptr);
    if(texture) {
        sprite = new sf::Sprite();
        sprite->setTexture(*texture);
        sprite->setScale(uiScaleX, uiScaleY);
        spriteList.push_back(sprite);
    }
    return sprite;
}

sf::Text* DisplaySystem::CreateText(sf::Font* font) {
    sf::Text* text(nullptr);
    if(font) {
        text = new sf::Text();
        text->setFont(*font);
        text->setScale(uiScaleX, uiScaleY);
        textList.push_back(text);
    }
    return text;
}

std::pair<float, float> DisplaySystem::GetUIScale() const {
    return std::make_pair<float,float>((const float)uiScaleX, (const float)uiScaleY);
}
