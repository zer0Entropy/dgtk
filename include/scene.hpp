//
// Created by zeroc00l on 10/31/23.
//

#include <memory>
#include <map>
#include "ui.hpp"
#include "decoration.hpp"
#include "map.hpp"
#include "input.hpp"

#ifndef DGTKPROJECT_SCENE_HPP
#define DGTKPROJECT_SCENE_HPP

struct Scene {
    std::vector<std::unique_ptr<uiObject>>      uiObjects;
    std::map<UniqueID, uiObjectProperties>      uiProperties;
    std::map<UniqueID, DecorationProperties>    decorationProperties;
    std::map<UniqueID, InputListener*>          keyListeners;
    std::unique_ptr<Map>                        map;
    std::vector<sf::Sprite*>                    spriteList;
    std::vector<sf::Text*>                      textList;
};

class Game;

class GameplayTransition: public InputListener {
public:
    GameplayTransition(Game* gamePtr);
    GameplayTransition(const GameplayTransition& copy) = default;
    ~GameplayTransition() = default;
    void ReceiveInput(const sf::Event& event);
private:
    Game* game;
};

#endif //DGTKPROJECT_SCENE_HPP
