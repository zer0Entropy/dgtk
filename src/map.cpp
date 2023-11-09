//
// Created by zeroc00l on 11/6/23.
//
#include <SFML/Graphics/Sprite.hpp>
#include "../include/map.hpp"

TerrainProperties ReadTerrainPropertiesFromJSON(const nlohmann::json& jsonDoc) {
    TerrainProperties terrainProperties;

    auto terrainJSON = jsonDoc.begin().value();
    auto findTerrainType = terrainJSON.find(TerrainPropertyNames.at((int) TerrainPropertyID::TerrainType));

    for (auto terrainPropertyIter = terrainJSON.begin(); terrainPropertyIter != terrainJSON.end(); ++terrainPropertyIter) {

        for (int terrainKeyIndex = 0; terrainKeyIndex < (int) TerrainPropertyID::TotalNumTerrainPropertyIDs; ++terrainKeyIndex) {
            TerrainPropertyID keyID((TerrainPropertyID) terrainKeyIndex);
            std::string_view key(TerrainPropertyNames.at(terrainKeyIndex));
            if (terrainPropertyIter.key() == key) {
                if (keyID == TerrainPropertyID::TerrainType) {
                    std::string terrainString = terrainPropertyIter.value();
                    if(terrainString.compare("floor") == 0) {
                        terrainProperties.terrainType = TerrainType::Floor;
                    } else if(terrainString.compare("wall") == 0) {
                        terrainProperties.terrainType = TerrainType::Wall;
                    }
                } else if(keyID == TerrainPropertyID::Walkable) {
                    terrainProperties.isWalkable = terrainPropertyIter.value();
                } else if(keyID == TerrainPropertyID::TexturePosition) {
                    auto positionJSON = terrainPropertyIter.value();
                    auto findX = positionJSON.find("x");
                    auto findY = positionJSON.find("y");
                    if(findX != positionJSON.end()) {
                        terrainProperties.texturePosition.x = findX->get<int>();
                    }
                    if(findY != positionJSON.end()) {
                        terrainProperties.texturePosition.x = findY->get<int>();
                    }
                }
            }
            } // if(terrainPropertyIter.key == key)
        } // for(terrainKeyIndex)

    return terrainProperties;
}

nlohmann::json WriteTerrainPropertiesToJSON(const TerrainProperties& terrainProperties) {
    nlohmann::json terrainJSON;
    return terrainJSON;
}

void GenerateMap(Map* map) {
    const MapProperties& properties(map->properties);
    const TilePlacementStrategy& strategy(properties.strategy);

    TerrainType terrainMap[properties.height][properties.width];

    for(int y = 0; y < properties.height; ++y) {
        for(int x = 0; x < properties.width; ++x) {
            if(y == 0) {
                terrainMap[y][x] = strategy.edges[(int)Direction::Up];
            } else if( y == properties.height - 1) {
                terrainMap[y][x] = strategy.edges[(int)Direction::Down];
            } else if( x == 0) {
                terrainMap[y][x] = strategy.edges[(int)Direction::Left];
            } else if( x == properties.width - 1) {
                terrainMap[y][x] = strategy.edges[(int)Direction::Right];
            } else {
                terrainMap[y][x] = strategy.defaultTerrainType;
            }
        }
    }

    for(int y = 0; y < properties.height; ++y) {
        for(int x = 0; x < properties.width; ++x) {
            Tile& tile( map->tileArray[y][x] );
            TerrainType terrainType = terrainMap[y][x];
            tile.terrain = &map->properties.terrainProperties.at(terrainType);
            tile.sprite.reset(new sf::Sprite);
            tile.creature = nullptr;
            tile.isVisible = true;
        }
    }
}

MapProperties ReadMapPropertiesFromJSON(const nlohmann::json& jsonDoc) {
    MapProperties mapProperties;

    auto mapJSON = jsonDoc.begin().value();

    for (auto mapPropertyIter = mapJSON.begin(); mapPropertyIter != mapJSON.end(); ++mapPropertyIter) {

        for (int mapKeyIndex = 0; mapKeyIndex < (int) MapPropertyID::TotalNumMapPropertyIDs; ++mapKeyIndex) {
            MapPropertyID keyID((MapPropertyID) mapKeyIndex);
            std::string_view key(MapPropertyNames.at(mapKeyIndex));
            if (mapPropertyIter.key() == key) {
                if (keyID == MapPropertyID::MapName) {
                    mapProperties.name = mapPropertyIter.value();
                } // if(keyID == MapID)
                else if(keyID == MapPropertyID::Width) {
                    mapProperties.width = mapPropertyIter.value();
                }
                else if(keyID == MapPropertyID::Height) {
                    mapProperties.height = mapPropertyIter.value();
                }
                else if(keyID == MapPropertyID::TexturePath) {
                    mapProperties.texturePath = mapPropertyIter.value();
                }
                else if(keyID == MapPropertyID::TextureWidth) {
                    mapProperties.textureWidth = mapPropertyIter.value();
                }
                else if(keyID == MapPropertyID::TextureHeight) {
                    mapProperties.textureHeight = mapPropertyIter.value();
                }
                else if(keyID == MapPropertyID::TerrainProperties) {
                    auto terrainJSON = mapPropertyIter.value();
                    for(auto terrainIter = terrainJSON.begin(); terrainIter != terrainJSON.end(); ++terrainIter) {
                        TerrainProperties terrainProperties = ReadTerrainPropertiesFromJSON(terrainJSON);
                        mapProperties.terrainProperties.insert(std::make_pair(terrainProperties.terrainType, terrainProperties));
                    }
                }
                else if(keyID == MapPropertyID::TilePlacementStrategy) {
                    auto tilePlacementJSON = mapPropertyIter.value();
                    for(auto tilePlacementIter = tilePlacementJSON.begin();
                        tilePlacementIter != tilePlacementJSON.end(); ++tilePlacementIter) {
                        if (keyID == MapPropertyID::DefaultTerrain) {
                            std::string defaultTerrainString = tilePlacementIter.value();
                            for(int terrainTypeIndex = 0; terrainTypeIndex < (int)TerrainType::TotalNumTerrainTypes; ++terrainTypeIndex) {
                                if(defaultTerrainString.compare(TerrainTypeNames.at(terrainTypeIndex)) == 0) {
                                    mapProperties.strategy.defaultTerrainType = (TerrainType)terrainTypeIndex;
                                }
                            } // for each terrainType
                        } else if (keyID == MapPropertyID::NorthEdge) {
                            std::string edgeString = tilePlacementIter.value();
                            for(int terrainTypeIndex = 0; terrainTypeIndex < (int)TerrainType::TotalNumTerrainTypes; ++terrainTypeIndex) {
                                if(edgeString.compare(TerrainTypeNames.at(terrainTypeIndex)) == 0) {
                                    mapProperties.strategy.edges[(int)Direction::Up] = (TerrainType)terrainTypeIndex;
                                }
                            } // for each terrainType
                        } else if (keyID == MapPropertyID::SouthEdge) {
                            std::string edgeString = tilePlacementIter.value();
                            for(int terrainTypeIndex = 0; terrainTypeIndex < (int)TerrainType::TotalNumTerrainTypes; ++terrainTypeIndex) {
                                if(edgeString.compare(TerrainTypeNames.at(terrainTypeIndex)) == 0) {
                                    mapProperties.strategy.edges[(int)Direction::Down] = (TerrainType)terrainTypeIndex;
                                }
                            } // for each terrainType
                        } else if (keyID == MapPropertyID::WestEdge) {
                            std::string edgeString = tilePlacementIter.value();
                            for(int terrainTypeIndex = 0; terrainTypeIndex < (int)TerrainType::TotalNumTerrainTypes; ++terrainTypeIndex) {
                                if(edgeString.compare(TerrainTypeNames.at(terrainTypeIndex)) == 0) {
                                    mapProperties.strategy.edges[(int)Direction::Left] = (TerrainType)terrainTypeIndex;
                                }
                            } // for each terrainType
                        } else if (keyID == MapPropertyID::EastEdge) {
                            std::string edgeString = tilePlacementIter.value();
                            for(int terrainTypeIndex = 0; terrainTypeIndex < (int)TerrainType::TotalNumTerrainTypes; ++terrainTypeIndex) {
                                if(edgeString.compare(TerrainTypeNames.at(terrainTypeIndex)) == 0) {
                                    mapProperties.strategy.edges[(int)Direction::Right] = (TerrainType)terrainTypeIndex;
                                }
                            } // for each terrainType
                        }
                    }
                }
            } // if(mapPropertyIter.key() == key)
        } // for(mapKeyIndex)

    }  // for(mapPropertyIter)
    
    return mapProperties;
}

nlohmann::json WriteMapPropertiesToJSON(const MapProperties& mapProperties) {
    nlohmann::json mapJSON;
    return mapJSON;
}
