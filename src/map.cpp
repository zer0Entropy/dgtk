//
// Created by zeroc00l on 11/6/23.
//
#include <SFML/Graphics/Sprite.hpp>
#include "../include/map.hpp"
#include "../include/game.hpp"
#include "../include/log.hpp"
#include "../include/dijkstra.hpp"

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

void GenerateMap(Map* map, const DisplayConfig& displayConfig) {
    const MapProperties& properties(map->properties);
    TerrainType terrainMap[properties.height][properties.width];

    // First pass: make everything a Wall
    for(int y = 0; y < properties.height; ++y) {
        for (int x = 0; x < properties.width; ++x) {
            terrainMap[y][x] = TerrainType::Wall;
        } // x
    } // y

    // Second pass: change each tile within a Room into Floor
    for(const auto& roomKeyValuePair : map->properties.roomList) {
        const Room& room(roomKeyValuePair.second);
        const MapLocation& topLeft(room.topLeft);
        for(int y = topLeft.y + 1; y < topLeft.y + room.height - 1; ++y) {
            for(int x = topLeft.x + 1; x < topLeft.x + room.width - 1; ++x) {
                terrainMap[y][x] = TerrainType::Floor;
            } // x
        } // y
    } // for each Room

    TerrainProperties& floor(map->properties.terrainProperties.at(TerrainType::Floor));
    TerrainProperties& wall(map->properties.terrainProperties.at(TerrainType::Wall));
    float scaleX = displayConfig.tileScaleX;
    float scaleY = displayConfig.tileScaleY;

    // Final pass: create a Tile at each MapLocation, create its Sprite & set properties
    for(int y = 0; y < properties.height; ++y) {
        for(int x = 0; x < properties.width; ++x) {
            Tile& tile( map->tileArray[y][x] );
            TerrainType terrainType = terrainMap[y][x];
            tile.terrain = &map->properties.terrainProperties.at(terrainType);
            tile.sprite.reset(new sf::Sprite);
            tile.sprite->setTexture(*tile.terrain->texture);
            tile.sprite->setScale(scaleX, scaleY);
            tile.isVisible = true;

            /* DEBUG */
            tile.terrain->isWalkable = true;
            /* DEBUG */
        }
    }

}

Path CreateHallway(Map& map, Dijkstra::DistanceMap& distanceMap, const MapLocation& origin, const MapLocation& destination) {
    TerrainProperties* floor = (TerrainProperties*)(&map.properties.terrainProperties.at(TerrainType::Floor));
    Path hallway = distanceMap.FindPath(destination);
    for(MapLocation& step : hallway.steps) {
        map.tileArray[step.y][step.x].terrain = floor;
        map.tileArray[step.y][step.x].sprite->setTexture(*floor->texture);
    }
    return hallway;
}

void CreateHallways(Map& map) {
    for(auto& roomKeyValuePair : map.properties.roomList) {
        UniqueID roomID(roomKeyValuePair.first);
        Room& room(roomKeyValuePair.second);
        int shortestDistance(999);
        UniqueID nearestRoomID("");
        TerrainProperties* floor = (TerrainProperties*)(&map.properties.terrainProperties.at(TerrainType::Floor));

        Dijkstra::DistanceMap distanceMap;
        Path path;
        distanceMap.Generate(room.center, map.properties.width, map.properties.height);
        for(auto& room2KeyValuePair : map.properties.roomList) {
            UniqueID room2ID(room2KeyValuePair.first);
            Room& room2(room2KeyValuePair.second);
            if(room.center.x == room2.center.x && room.center.y == room2.center.y) { continue; }

            path = distanceMap.FindPath(room2.center);
            room.pathsToOtherRooms.insert(std::make_pair(room2ID, path));
            if(path.steps.size() < shortestDistance) {
                shortestDistance = path.steps.size();
                nearestRoomID = room2ID;
            }
        } // room2

        Path hallway = CreateHallway(map, distanceMap, room.center, map.properties.roomList.at(nearestRoomID).center);
        map.properties.hallwayList.push_back(hallway);
    } // room
}

bool DoesPathContain(const Map& map, const Path& path, TerrainType terrain) {
    bool containsTerrain(false);

    for(auto pathIter = path.steps.begin(); !containsTerrain && pathIter != path.steps.end(); ++pathIter) {
        const MapLocation location(*pathIter);
        const Tile& tile(map.tileArray[location.y][location.x]);
        if(tile.terrain->terrainType == terrain) {
            containsTerrain = true;
        }
    }

    return containsTerrain;
}
