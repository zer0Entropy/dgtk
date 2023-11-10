//
// Created by zeroc00l on 10/31/23.
//

#include <memory>
#include <map>
#include <nlohmann/json.hpp>
#include "ui.hpp"
#include "decoration.hpp"
#include "map.hpp"
#include "input.hpp"
#include "player.hpp"
#include "view.hpp"

#ifndef DGTKPROJECT_SCENE_HPP
#define DGTKPROJECT_SCENE_HPP

const std::string SceneHeader{"Scene"};

enum class ScenePropertyID {
    SceneID = 0,
    uiObjects,
    uiObjectProperties,
    DecorationProperties,
    Map,
    MapView,
    TotalNumScenePropertyIDs
};

const std::vector<std::string> ScenePropertyNames{
        {"scene_id"},
        {"ui_objects"},
        {"ui_object_properties"},
        {"decoration_properties"},
        {"map_properties"},
        {"map_view_properties"}
};

struct SceneProperties {
    UniqueID            id;
    std::vector<uiObjectProperties>             uiObjProperties;
    std::vector<DecorationProperties>           decorationProperties;
    MapProperties                               mapProperties;
    MapViewProperties                           viewProperties;
};

struct Scene {
    SceneProperties                             properties;
    std::vector<Creature*>                      creatures;
    std::vector<uiObject*>                      uiObjects;
    std::map<UniqueID, InputListener*>          keyListeners;
    std::unique_ptr<Map>                        map;
    MapView*                                    mapView;
    Scene();
    Scene(const Scene& copy) = default;
    ~Scene() = default;
};

SceneProperties ReadScenePropertiesFromJSON(const nlohmann::json& jsonDoc, Game* game);
nlohmann::json WriteScenePropertiesToJSON(const SceneProperties& sceneProperties);

class SceneTransition: public InputListener {
public:
    SceneTransition(uiActionTrigger triggerType, UniqueID transitionSceneID, Game* gamePtr);
    SceneTransition(const SceneTransition& copy) = default;
    ~SceneTransition() = default;
    void ReceiveInput(const sf::Event& event) override;
private:
    uiActionTrigger trigger;
    UniqueID sceneID;
    Game* game;
};

#endif //DGTKPROJECT_SCENE_HPP
