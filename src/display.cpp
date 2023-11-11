//
// Created by zeroc00l on 10/30/23.
//


#include "../include/display.hpp"
#include "../include/scene.hpp"
#include "../include/game.hpp"

DisplaySystem::DisplaySystem(const DisplayConfig& config, Game* gamePtr):
    System(SystemID::Display), game(gamePtr),
    windowProperties(config.windowProperties) {

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
        RenderScene(currentScene);
        DrawScene();

        if(currentScene->map) {
            DrawView(*currentScene->mapView, *currentScene->map.get());
        }

        window->display();
        for(int index = 0; index < (int)LayerID::TotalNumLayerIDs; ++index) {
            std::vector<sf::Drawable*>& currentLayer = drawLayers[index];
            currentLayer.clear();
        }
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
    window = std::make_unique<sf::RenderWindow>(sf::VideoMode(properties.width, properties.height), properties.title, sf::Style::None );
}

void DisplaySystem::CloseWindow() {
    if(window) {
        window->close();
    }
}

sf::RenderWindow* DisplaySystem::GetWindow() const {
    return window.get();
}

void DisplaySystem::RenderScene(Scene* scene) {
    for(int index = 0; index < (int)LayerID::TotalNumLayerIDs; ++index) {
        std::vector<sf::Drawable*>& currentLayer = drawLayers[index];
        for(auto uiObjPtr: scene->uiObjects) {
            if(uiObjPtr->uiProperties.layer == (LayerID)index) {
                if(uiObjPtr->sprite) {
                    currentLayer.push_back(uiObjPtr->sprite.get());
                }
                if(uiObjPtr->text) {
                    currentLayer.push_back(uiObjPtr->text.get());
                }
                if(!uiObjPtr->children.empty()) {
                    for(auto& childPtr : uiObjPtr->children) {
                        if(childPtr->sprite) {
                            currentLayer.push_back(childPtr->sprite.get());
                        }
                        if(childPtr->text) {
                            currentLayer.push_back(childPtr->text.get());
                        }
                    }
                }
            } // if(layer == currentLayer)
        } // for each uiObject
    } // for each layer
}

void DisplaySystem::DrawScene() {
    for(int index = 0; index < (int)LayerID::TotalNumLayerIDs; ++index) {
        std::vector<sf::Drawable*>& currentLayer = drawLayers[index];
        for(auto drawable : currentLayer) {
            window->draw(*drawable);
        } // for each drawable in currentLayer
    } // for each layer
}

void DisplaySystem::DrawView(const MapView& view, const Map& map) {
    int left(0);
    int top(0);
    const MapViewProperties& viewProperties(view.properties);
    if(view.properties.centerLocation.x > viewProperties.widthInTiles / 2) {
        left = viewProperties.centerLocation.x - (viewProperties.widthInTiles / 2);
    }
    if(viewProperties.centerLocation.y > viewProperties.heightInTiles / 2) {
        top = viewProperties.centerLocation.y - (viewProperties.heightInTiles / 2);
    }

    int tileWidth(map.properties.textureWidth * game->GetDisplayConfig().tileScaleX);
    int tileHeight(map.properties.textureHeight * game->GetDisplayConfig().tileScaleY);
    for(int y = top; y < top + viewProperties.heightInTiles; ++y) {
        for(int x = left; x < left + viewProperties.widthInTiles; ++x) {
            const Tile& tile(map.tileArray[y][x]);
            sf::Sprite* sprite(tile.sprite.get());
            if(sprite != nullptr) {
                sprite->setPosition((x - left) * tileWidth, (y - top) * tileHeight);
                window->draw(*tile.sprite);
                if (tile.creature) {
                    tile.creature->sprite->setPosition((x - left) * tileWidth, (y - top) * tileHeight);
                    window->draw(*tile.creature->sprite);
                }
            }
        }
    }
}
