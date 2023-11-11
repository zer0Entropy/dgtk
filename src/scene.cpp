// Created by zeroc00l on 11/1/23.
//
#include <SFML/Window/Event.hpp>
#include <fstream>
#include "../include/scene.hpp"
#include "../include/game.hpp"
#include "../include/log.hpp"
#include "../include/math.hpp"

SceneProperties ReadScenePropertiesFromJSON(const nlohmann::json& jsonDoc, Game* game) {
    SceneProperties sceneProperties;
    auto sceneJSON = jsonDoc.begin().value();
    auto findSceneID = sceneJSON.find(ScenePropertyNames.at((int) ScenePropertyID::SceneID));

    LogSystem* logSystem = game->GetLogSystem();
    std::string message("");

    for (auto scenePropertyIter = sceneJSON.begin(); scenePropertyIter != sceneJSON.end(); ++scenePropertyIter) {

        for (int sceneKeyIndex = 0; sceneKeyIndex < (int) ScenePropertyID::TotalNumScenePropertyIDs; ++sceneKeyIndex) {
            ScenePropertyID keyID((ScenePropertyID) sceneKeyIndex);
            std::string_view key(ScenePropertyNames.at(sceneKeyIndex));
            if (scenePropertyIter.key() == key) {
                if (keyID == ScenePropertyID::SceneID) {
                    sceneProperties.id = scenePropertyIter.value();
                } // if(keyID == SceneID)
                else if (keyID == ScenePropertyID::uiObjects) {
                    const nlohmann::json uiObjListJSON = scenePropertyIter.value();
                    for (auto uiObjListIter = uiObjListJSON.begin();
                         uiObjListIter != uiObjListJSON.end(); ++uiObjListIter) {

                        uiObjectProperties uiObjProperties;
                        DecorationProperties decProperties;

                        auto propertyListsJSON = uiObjListIter.value();
                        for (auto propertyListIter = propertyListsJSON.begin();
                             propertyListIter != propertyListsJSON.end(); ++propertyListIter) {
                            std::string_view propertyListName = propertyListIter.key();
                            auto propertyList = propertyListIter.value();

                            // {propertyListName: decoration_properties propertyList: [dec_properties]}
                            if (propertyListName.compare(
                                    ScenePropertyNames[(int) ScenePropertyID::DecorationProperties]) == 0) {
                                decProperties = ReadDecorationPropertiesFromJSON(propertyList, game);
                            }

                                // {propertyListName: ui_object_properties propertyList: [ui_obj_properties]}
                            else if (propertyListName.compare(
                                    ScenePropertyNames[(int) ScenePropertyID::uiObjectProperties]) == 0) {
                                uiObjProperties = ReadUIObjPropertiesFromJSON(propertyList,game->GetMathParser());
                            }

                            if(decProperties.id.length() > 0 && decProperties.id == uiObjProperties.id) {
                                sceneProperties.decorations.push_back(std::make_pair(uiObjProperties, decProperties));
                                decProperties.id = uiObjProperties.id = "";
                            }

                        } // for(propertyListIter)
                    } // for(uiObjListIter)
                } // else if(keyID == uiObjects)
                else if(keyID == ScenePropertyID::Map) {
                    auto mapJSON = scenePropertyIter.value();
                    sceneProperties.mapProperties = ReadMapPropertiesFromJSON(mapJSON, game);
                }  // else if(keyID == Map)
                else if(keyID == ScenePropertyID::MapView) {
                    auto mapViewJSON = scenePropertyIter.value();
                    sceneProperties.viewProperties = ReadMapViewPropertiesFromJSON(mapViewJSON, game);
                }  // else if(keyID == MapView)
                else if(keyID == ScenePropertyID::Actions) {
                    const nlohmann::json actionListJSON = scenePropertyIter.value();
                    for (auto actionIter = actionListJSON.begin(); actionIter != actionListJSON.end(); ++actionIter) {
                        const nlohmann::json actionJSON = actionIter.value();
                        Action action = ReadActionFromJSON(actionJSON, game);
                        sceneProperties.actions.insert(std::make_pair(action.id, action));
                    }
                } // else if(keyID == Actions)
            } // if(findKey)
        } // for(sceneKeyIndex)

    } // for(scenePropertyIter)
    return sceneProperties;
}

nlohmann::json WriteScenePropertiesToJSON(const SceneProperties& sceneProperties) {
    nlohmann::json sceneJSON;
    return sceneJSON;
}

Scene::Scene() {
    properties.id = "";
    properties.mapProperties.name = "";
    mapView = nullptr;
}
