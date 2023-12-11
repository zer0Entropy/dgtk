//
// Created by zeroc00l on 11/1/23.
//

#include "dijkstra.hpp"
#include "map.hpp"
#include "ui.hpp"

#ifndef DGTKPROJECT_VIEW_HPP
#define DGTKPROJECT_VIEW_HPP

constexpr int VisibilityThreshold{6};

struct MapViewProperties {
    int widthInPixels, heightInPixels;
    int widthInTiles, heightInTiles;
    MapLocation centerLocation;
    Position position;
};

struct MapView {
    MapViewProperties properties;
    Dijkstra::WeightedDistanceMap visibilityMap;
};

MapViewProperties ReadMapViewPropertiesFromJSON(const nlohmann::json& mapViewJSON, Game* game);

void CenterViewOnPlayer(MapView& view, const Map& map, MapLocation playerLocation);

void InitVisibilityMap(const Map& map, Dijkstra::WeightedDistanceMap& visibilityMap);
void UpdateVisibilityMap(Map& map, Dijkstra::WeightedDistanceMap& visibilityMap, const MapLocation& center);

#endif //DGTKPROJECT_VIEW_HPP
