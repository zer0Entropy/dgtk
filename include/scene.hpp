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
    Actions,
    Map,
    MapView,
    TotalNumScenePropertyIDs
};

const std::vector<std::string> ScenePropertyNames{
        {"scene_id"},
        {"ui_objects"},
        {"ui_object_properties"},
        {"decoration_properties"},
        {"actions"},
        {"map_properties"},
        {"map_view_properties"}
};

struct SceneProperties {
    UniqueID            id;
    std::vector<std::pair<uiObjectProperties,DecorationProperties>> decorations;
    MapProperties                               mapProperties;
    MapViewProperties                           viewProperties;
    std::map<UniqueID, Action>                  actions;
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
/*
class SceneTransition: public InputListener {
public:
    SceneTransition(ActionTrigger triggerType, UniqueID transitionSceneID, Game* gamePtr);
    SceneTransition(const SceneTransition& copy) = default;
    ~SceneTransition() = default;
    void ReceiveInput(const sf::Event& event) override;
private:
    ActionTrigger trigger;
    UniqueID sceneID;
    Game* game;
};
*/
#endif //DGTKPROJECT_SCENE_HPP
