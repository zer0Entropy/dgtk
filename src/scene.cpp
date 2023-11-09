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
                        auto propertyListsJSON = uiObjListIter.value();
                        for (auto propertyListIter = propertyListsJSON.begin();
                             propertyListIter != propertyListsJSON.end(); ++propertyListIter) {
                            auto propertyListName = propertyListIter.key();
                            auto propertyList = propertyListIter.value();

                            // {propertyListName: decoration_properties propertyList: [dec_properties]}
                            if (propertyListName.compare(
                                    ScenePropertyNames[(int) ScenePropertyID::DecorationProperties]) == 0) {
                                DecorationProperties decProperties = ReadDecorationPropertiesFromJSON(propertyList);
                                sceneProperties.decorationProperties.push_back(decProperties);
                            }

                                // {propertyListName: ui_object_properties propertyList: [ui_obj_properties]}
                            else if (propertyListName.compare(
                                    ScenePropertyNames[(int) ScenePropertyID::uiObjectProperties]) == 0) {
                                uiObjectProperties uiObjProperties = ReadUIObjPropertiesFromJSON(propertyList,
                                                                                                 game->GetMathParser());
                                sceneProperties.uiObjProperties.push_back(uiObjProperties);
                            }

                        } // for(propertyListIter)
                    } // for(uiObjListIter)
                } // else if(keyID == uiObjects)
            } // if(findKey)
        } // for(sceneKeyIndex)

    } // for(scenePropertyIter)
    return sceneProperties;
}

nlohmann::json WriteScenePropertiesToJSON(const SceneProperties& sceneProperties) {
    nlohmann::json sceneJSON;
    return sceneJSON;
}

SceneTransition::SceneTransition(uiActionTrigger triggerType, std::string_view pathToScene, Game* gamePtr):
    InputListener(gamePtr), trigger(triggerType), scenePath(pathToScene), game(gamePtr) {

}

void SceneTransition::ReceiveInput(const sf::Event& event) {
    Scene* scene(nullptr);

    if(trigger == uiActionTrigger::OnKeyPress) {

        if (event.type == sf::Event::EventType::KeyPressed) {
            scene = game->GetResourceSystem()->LoadScene(scenePath, *game);
        } // sf::Event::KeyPressed

    } // trigger: OnKeyPress

    if(scene) {
        game->TransitionTo(scene);
    }
}

