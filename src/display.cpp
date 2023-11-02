//
// Created by zeroc00l on 10/30/23.
//


#include "../include/display.hpp"
#include "../include/scene.hpp"
#include "../include/game.hpp"

DisplaySystem::DisplaySystem(const DisplayConfig& config, Game* gamePtr):
    System(SystemID::Display), game(gamePtr),
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
        Scene* currentScene = game->GetCurrentScene();
        DrawScene(currentScene);
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

void DisplaySystem::DrawScene(Scene* scene) {
    auto &spriteList(scene->spriteList);
    for(auto spriteIter = spriteList.begin(); spriteIter != spriteList.end(); ++spriteIter) {
        window->draw(**spriteIter);
    }
    auto &textList(scene->textList);
    for(auto textIter = textList.begin(); textIter != textList.end(); ++textIter) {
        window->draw(**textIter);
    }

    auto &creatureList(scene->creatures);
    for(auto creatureIter = creatureList.begin(); creatureIter != creatureList.end(); ++creatureIter) {
        window->draw(*(*creatureIter)->sprite);
    }
}

std::pair<float, float> DisplaySystem::GetUIScale() const {
    return std::make_pair<float,float>((const float)uiScaleX, (const float)uiScaleY);
}
