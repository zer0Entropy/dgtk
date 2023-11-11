//
// Created by zeroc00l on 11/10/23.
//
#include "../include/creature.hpp"

CreatureProperties ReadCreaturePropertiesFromJSON(const nlohmann::json& creatureJSON) {
    CreatureProperties creatureProperties;
    for(int creaturePropertyIndex = 0;
    creaturePropertyIndex < (int)CreaturePropertyID::TotalNumCreaturePropertyIDs; ++creaturePropertyIndex) {
        CreaturePropertyID propertyID((CreaturePropertyID)creaturePropertyIndex);
        auto findProperty = creatureJSON.find(CreaturePropertyNames.at(creaturePropertyIndex));
        if(findProperty != creatureJSON.end()) {
            switch (propertyID) {
                case CreaturePropertyID::Name:
                    creatureProperties.name = findProperty->get<std::string>();
                    break;
                case CreaturePropertyID::CreatureType: {
                    std::string typeString = findProperty->get<std::string>();
                    if(typeString.compare(CreatureTypeNames.at((int)CreatureType::Human)) == 0) {
                        creatureProperties.type = CreatureType::Human;
                    }
                    break;
                }
                case CreaturePropertyID::Location: {
                    auto locationJSON = *findProperty;
                    auto findX = locationJSON.find("x");
                    auto findY = locationJSON.find("y");
                    if(findX != locationJSON.end()) {
                        creatureProperties.location.x = findX->get<int>();
                    }
                    if(findY != locationJSON.end()) {
                        creatureProperties.location.x = findY->get<int>();
                    }
                    break;
                }
                case CreaturePropertyID::TextureID:
                    creatureProperties.textureID = findProperty->get<std::string>();
                    break;
                case CreaturePropertyID::TexturePath:
                    creatureProperties.texturePath = findProperty->get<std::string>();
                    break;
                case CreaturePropertyID::TexturePosition: {
                    auto positionJSON = *findProperty;
                    auto findX = positionJSON.find("x");
                    auto findY = positionJSON.find("y");
                    if(findX != positionJSON.end()) {
                        creatureProperties.texturePosition.x = findX->get<int>();
                    }
                    if(findY != positionJSON.end()) {
                        creatureProperties.texturePosition.y = findY->get<int>();
                    }
                    break;
                }
                case CreaturePropertyID::TotalNumCreaturePropertyIDs:   default:        break;
            } // switch(propertyID)
        } // if(findProperty)
    } // for(creaturePropertyIndex)
    return creatureProperties;
}

nlohmann::json WriteCreaturePropertiesToJSON(const CreatureProperties& creatureProperties) {
    nlohmann::json creatureJSON;
    return creatureJSON;
}
