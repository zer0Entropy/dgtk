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

enum class uiActionType {
    None,
    TransitionToScene
};

enum class uiActionTrigger {
    None,
    OnKeyPress
};

enum class uiLayerID {
    Background = 0,
    WindowFrame,
    Sprite,
    Text,
    TotalNumUILayers
};

class uiObject;

struct uiAction {
    uiActionType    type;
    uiActionTrigger trigger;
    std::string     resourcePath;
    UniqueID        actorID;
    UniqueID        targetID;
};

struct uiObjectProperties {
    uiObjectType    uiType;
    TextureSource   textureSource;
    Alignment       align;
    Position        origin;
    Position        position;
    uiLayerID       layer;
};

struct uiObject {
    UniqueID                                id;
    uiObjectProperties                      uiProperties;
    uiAction                                action;
    std::vector<std::unique_ptr<uiObject>>  children;
    std::unique_ptr<sf::Sprite>             sprite;
    std::unique_ptr<sf::Text>               text;

    explicit uiObject(UniqueID assignID, uiObjectType assignType):
        id(assignID), sprite(nullptr), text(nullptr) {
        uiProperties.uiType = assignType;
    }
};

#endif //DGTKPROJECT_UI_HPP
