//
// Created by zeroc00l on 10/30/23.
//

#include <vector>
#include <SFML/Graphics/Sprite.hpp>
#include <SFML/Graphics/Text.hpp>
#include "resource.hpp"

#ifndef DGTKPROJECT_UI_HPP
#define DGTKPROJECT_UI_HPP

enum class uiObjectType {
    Decoration,
    Toolbar,
    Scrollbar
};

struct uiObjectProperties {
    uiObjectType    uiType;
    TextureSource   textureSource;
    Alignment       align;
    Position        origin;
    Position        position;
};

struct uiObject {
    UniqueID                                id;
    uiObjectProperties                      uiProperties;
    std::vector<std::unique_ptr<uiObject>>  children;
    std::unique_ptr<sf::Sprite>             sprite;
    std::unique_ptr<sf::Text>               text;

    explicit uiObject(UniqueID assignID, uiObjectType assignType):
        id(assignID), sprite(nullptr), text(nullptr) {
        uiProperties.uiType = assignType;
    }
};

#endif //DGTKPROJECT_UI_HPP
