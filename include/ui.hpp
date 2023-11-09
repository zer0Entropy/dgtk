//
// Created by zeroc00l on 10/30/23.
//

#include <vector>
#include <SFML/Graphics/Sprite.hpp>
#include <SFML/Graphics/Text.hpp>
#include <nlohmann/json.hpp>
#include "resource.hpp"
#include "math.hpp"
#include "layer.hpp"

#ifndef DGTKPROJECT_UI_HPP
#define DGTKPROJECT_UI_HPP

enum class uiPropertyID {
        ObjectID = 0,
        ObjectType,
        Alignment,
        Origin,
        Position,
        LayerID,
        TexturePath,
        TexturePosition,
        TextureWidth,
        TextureHeight,
        ActionType,
        ActionTrigger,
        ResourcePath,
        TotalNumUIPropertyIDs
};

const std::vector<std::string> uiPropertyNames{
        {"object_id"},
        {"object_type"},
        {"alignment"},
        {"origin"},
        {"position"},
        {"layer_id"},
        {"texture_path"},
        {"texture_position"},
        {"texture_width"},
        {"texture_height"},
        {"action_type"},
        {"action_trigger"},
        {"resource_path"}
};

enum class uiObjectType {
    Decoration,
    Toolbar,
    Scrollbar,
    MapView
};

enum class uiActionType {
    None,
    TransitionToScene
};

enum class uiActionTrigger {
    None,
    OnKeyPress
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
    UniqueID        id;
    uiObjectType    uiType;
    TextureSource   textureSource;
    Alignment       align;
    Position        origin;
    Position        position;
    LayerID       layer;
    uiActionType    actionType;
    uiActionTrigger actionTrigger;
    std::string     resourcePath;
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

uiObjectProperties ReadUIObjPropertiesFromJSON(const nlohmann::json& jsonDoc, MathParser& mathParser);
nlohmann::json WriteUIObjPropertiesToJSON(const uiObjectProperties& uiObjProperties);

#endif //DGTKPROJECT_UI_HPP
