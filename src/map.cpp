//
// Created by zeroc00l on 11/6/23.
//
#include <SFML/Graphics/Sprite.hpp>
#include "../include/map.hpp"
#include "../include/game.hpp"
#include "../include/log.hpp"

TerrainProperties ReadTerrainPropertiesFromJSON(const nlohmann::json& terrainJSON, Game* game) {
    TerrainProperties terrainProperties;

    auto findTerrainType = terrainJSON.find(TerrainPropertyNames.at((int) TerrainPropertyID::TerrainType));
    auto findWalkable = terrainJSON.find(TerrainPropertyNames.at((int)TerrainPropertyID::Walkable));
    auto findTexturePosition = terrainJSON.find(TerrainPropertyNames.at((int)TerrainPropertyID::TexturePosition));

    if(findTerrainType != terrainJSON.end()) {
        terrainProperties.name = findTerrainType.value();
        for(int terrainTypeIndex = 0; terrainTypeIndex < (int)TerrainType::TotalNumTerrainTypes; ++terrainTypeIndex) {
            if(terrainProperties.name.compare(TerrainTypeNames.at(terrainTypeIndex)) == 0) {
                terrainProperties.terrainType = (TerrainType)terrainTypeIndex;
            }
        }
    }
    if(findWalkable != terrainJSON.end()) {
        terrainProperties.isWalkable = findWalkable.value();
    }
    if(findTexturePosition != terrainJSON.end()) {
        auto positionJSON = findTexturePosition.value();
        auto findX = positionJSON.find("x");
        auto findY = positionJSON.find("y");
        if(findX != positionJSON.end()) {
            terrainProperties.texturePosition.x = findX.value();
        }
        if(findY != positionJSON.end()) {
            terrainProperties.texturePosition.y = findY.value();
        }
    }

    return terrainProperties;
}

nlohmann::json WriteTerrainPropertiesToJSON(const TerrainProperties& terrainProperties) {
    nlohmann::json terrainJSON;
    return terrainJSON;
}

void GenerateMap(Map* map, const DisplayConfig& displayConfig) {
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

    float scaleX = displayConfig.tileScaleX;
    float scaleY = displayConfig.tileScaleY;

    for(int y = 0; y < properties.height; ++y) {
        for(int x = 0; x < properties.width; ++x) {
            Tile& tile( map->tileArray[y][x] );
            TerrainType terrainType = terrainMap[y][x];
            tile.terrain = &map->properties.terrainProperties.at(terrainType);
            tile.sprite.reset(new sf::Sprite);
            tile.sprite->setTexture(*tile.terrain->texture);
            tile.sprite->setScale(scaleX, scaleY);
            tile.isVisible = true;
        }
    }
}

MapProperties ReadMapPropertiesFromJSON(const nlohmann::json& jsonDoc, Game* game) {
    MapProperties mapProperties;
    MathParser& mathParser = game->GetMathParser();

    for (int mapPropertyIndex = 0; mapPropertyIndex < (int) MapPropertyID::TotalNumMapPropertyIDs; ++mapPropertyIndex) {
        MapPropertyID keyID((MapPropertyID) mapPropertyIndex);
        std::string_view key(MapPropertyNames.at(mapPropertyIndex));
        auto findKey = jsonDoc.find(key);
        if (findKey != jsonDoc.end()) {
            switch (keyID) {
                case MapPropertyID::MapName:
                    mapProperties.name = findKey->get<std::string>();
                    break;
                case MapPropertyID::Width:
                    mapProperties.width = findKey->get<int>();
                    break;
                case MapPropertyID::Height:
                    mapProperties.height = findKey->get<int>();
                    break;
                case MapPropertyID::TexturePath:
                    mapProperties.texturePath = findKey->get<std::string>();
                    break;
                case MapPropertyID::TextureWidth:
                    mapProperties.textureWidth = findKey->get<int>();
                    mathParser.RegisterVariable(MapPropertyNames.at((int)MapPropertyID::TextureWidth), mapProperties.textureWidth);
                    break;
                case MapPropertyID::TextureHeight:
                    mapProperties.textureHeight = findKey->get<int>();
                    mathParser.RegisterVariable(MapPropertyNames.at((int)MapPropertyID::TextureHeight), mapProperties.textureHeight);
                    break;
                case MapPropertyID::TerrainProperties: {
                    auto terrainJSON = *findKey;
                    for (auto terrainIter = terrainJSON.begin(); terrainIter != terrainJSON.end(); ++terrainIter) {
                        TerrainProperties terrainProperties = ReadTerrainPropertiesFromJSON(terrainIter.value(), game);
                        mapProperties.terrainProperties.insert(
                                std::make_pair(terrainProperties.terrainType, terrainProperties));
                    }
                    break;
                }
                case MapPropertyID::TilePlacementStrategy: {
                    mapProperties.strategy.edges[(int)Direction::None] = TerrainType::Empty;
                    auto tilePlacementJSON = *findKey;
                    auto findDefault = tilePlacementJSON.find(MapPropertyNames.at((int) MapPropertyID::DefaultTerrain));
                    auto findNorthEdge = tilePlacementJSON.find(MapPropertyNames.at((int) MapPropertyID::NorthEdge));
                    auto findSouthEdge = tilePlacementJSON.find(MapPropertyNames.at((int) MapPropertyID::SouthEdge));
                    auto findWestEdge = tilePlacementJSON.find(MapPropertyNames.at((int) MapPropertyID::WestEdge));
                    auto findEastEdge = tilePlacementJSON.find(MapPropertyNames.at((int) MapPropertyID::EastEdge));
                    if (findDefault != tilePlacementJSON.end()) {
                        std::string terrainTypeString = findDefault->get<std::string>();
                        if (terrainTypeString.compare(TerrainTypeNames.at((int) TerrainType::Empty)) == 0) {
                            mapProperties.strategy.defaultTerrainType = TerrainType::Empty;
                        } else if (terrainTypeString.compare(TerrainTypeNames.at((int) TerrainType::Floor)) == 0) {
                            mapProperties.strategy.defaultTerrainType = TerrainType::Floor;
                        } else if (terrainTypeString.compare(TerrainTypeNames.at((int) TerrainType::Wall)) == 0) {
                            mapProperties.strategy.defaultTerrainType = TerrainType::Wall;
                        }
                    }
                    if (findNorthEdge != tilePlacementJSON.end()) {
                        std::string terrainTypeString = findNorthEdge->get<std::string>();
                        if (terrainTypeString.compare(TerrainTypeNames.at((int) TerrainType::Empty)) == 0) {
                            mapProperties.strategy.edges[(int) Direction::Up] = TerrainType::Empty;
                        } else if (terrainTypeString.compare(TerrainTypeNames.at((int) TerrainType::Floor)) == 0) {
                            mapProperties.strategy.edges[(int) Direction::Up] = TerrainType::Floor;
                        } else if (terrainTypeString.compare(TerrainTypeNames.at((int) TerrainType::Wall)) == 0) {
                            mapProperties.strategy.edges[(int) Direction::Up] = TerrainType::Wall;
                        }
                    }
                    if (findSouthEdge != tilePlacementJSON.end()) {
                        std::string terrainTypeString = findSouthEdge->get<std::string>();
                        if (terrainTypeString.compare(TerrainTypeNames.at((int) TerrainType::Empty)) == 0) {
                            mapProperties.strategy.edges[(int) Direction::Down] = TerrainType::Empty;
                        } else if (terrainTypeString.compare(TerrainTypeNames.at((int) TerrainType::Floor)) == 0) {
                            mapProperties.strategy.edges[(int) Direction::Down] = TerrainType::Floor;
                        } else if (terrainTypeString.compare(TerrainTypeNames.at((int) TerrainType::Wall)) == 0) {
                            mapProperties.strategy.edges[(int) Direction::Down] = TerrainType::Wall;
                        }
                    }
                    if (findWestEdge != tilePlacementJSON.end()) {
                        std::string terrainTypeString = findWestEdge->get<std::string>();
                        if (terrainTypeString.compare(TerrainTypeNames.at((int) TerrainType::Empty)) == 0) {
                            mapProperties.strategy.edges[(int) Direction::Left] = TerrainType::Empty;
                        } else if (terrainTypeString.compare(TerrainTypeNames.at((int) TerrainType::Floor)) == 0) {
                            mapProperties.strategy.edges[(int) Direction::Left] = TerrainType::Floor;
                        } else if (terrainTypeString.compare(TerrainTypeNames.at((int) TerrainType::Wall)) == 0) {
                            mapProperties.strategy.edges[(int) Direction::Left] = TerrainType::Wall;
                        }
                    }
                    if (findEastEdge != tilePlacementJSON.end()) {
                        std::string terrainTypeString = findEastEdge->get<std::string>();
                        if (terrainTypeString.compare(TerrainTypeNames.at((int) TerrainType::Empty)) == 0) {
                            mapProperties.strategy.edges[(int) Direction::Right] = TerrainType::Empty;
                        } else if (terrainTypeString.compare(TerrainTypeNames.at((int) TerrainType::Floor)) == 0) {
                            mapProperties.strategy.edges[(int) Direction::Right] = TerrainType::Floor;
                        } else if (terrainTypeString.compare(TerrainTypeNames.at((int) TerrainType::Wall)) == 0) {
                            mapProperties.strategy.edges[(int) Direction::Right] = TerrainType::Wall;
                        }
                    }
                    break;
                }

            } // switch(keyID)
        } // if(findKey)
    } // for(mapPropertyIndex)

    return mapProperties;
}

nlohmann::json WriteMapPropertiesToJSON(const MapProperties& mapProperties) {
    nlohmann::json mapJSON;
    return mapJSON;
}
