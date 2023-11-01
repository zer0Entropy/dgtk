//
// Created by zeroc00l on 10/31/23.
//

#include <memory>
#include <map>
#include "ui.hpp"
#include "decoration.hpp"
#include "map.hpp"

#ifndef DGTKPROJECT_SCENE_HPP
#define DGTKPROJECT_SCENE_HPP

struct Scene {
    std::vector<std::unique_ptr<uiObject>>      uiObjects;
    std::map<UniqueID, uiObjectProperties>      uiProperties;
    std::map<UniqueID, DecorationProperties>    decorationProperties;
    Map                                         map;
    std::vector<sf::Sprite*>                    spriteList;
    std::vector<sf::Text*>                      textList;
};

#endif //DGTKPROJECT_SCENE_HPP
