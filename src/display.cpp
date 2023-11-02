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
        DrawUIObjects(currentScene->uiObjects);
        DrawView(currentScene->view, *currentScene->map.get());
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

void DisplaySystem::DrawUIObjects(const std::vector<std::unique_ptr<uiObject>>& uiObjectList) {
    for(auto iterator = uiObjectList.begin(); iterator != uiObjectList.end(); ++iterator) {
        uiObject *uiObj(&(*iterator->get()));
        if(uiObj->sprite) {
            window->draw(*uiObj->sprite);
        }
    } // draw Sprites
    for(auto iterator = uiObjectList.begin(); iterator != uiObjectList.end(); ++iterator) {
        uiObject* uiObj(&(*iterator->get()));
        if (!uiObj->children.empty()) {
            const auto& childList(uiObj->children);
            DrawUIObjects(childList);
        }
    } // draw Children
    for(auto iterator = uiObjectList.begin(); iterator != uiObjectList.end(); ++iterator) {
        uiObject *uiObj(&(*iterator->get()));
        if(uiObj->text) {
            window->draw(*uiObj->text);
        }
    } // draw Text
}

void DisplaySystem::DrawView(const MapView& view, const Map& map) {
    int left(0);
    int top(0);
    if(view.centerLocation.x > view.widthInTiles / 2) {
        left = view.centerLocation.x - (view.widthInTiles / 2);
    }
    if(view.centerLocation.y > view.heightInTiles / 2) {
        top = view.centerLocation.y - (view.heightInTiles / 2);
    }

    for(int y = top; y < top + view.heightInTiles; ++y) {
        for(int x = left; x < left + view.widthInTiles; ++x) {
            const Tile& tile(map.tileArray[y][x]);
            window->draw(*tile.sprite);
            if(tile.creature) {
                tile.creature->sprite->setPosition(tile.sprite->getPosition());
                window->draw(*tile.creature->sprite);
            }
        }
    }
}

std::pair<float, float> DisplaySystem::GetUIScale() const {
    return std::make_pair<float,float>((const float)uiScaleX, (const float)uiScaleY);
}
