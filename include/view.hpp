//
// Created by zeroc00l on 11/1/23.
//

#include "map.hpp"
#include "ui.hpp"

#ifndef DGTKPROJECT_VIEW_HPP
#define DGTKPROJECT_VIEW_HPP

struct MapViewProperties {
    int widthInPixels, heightInPixels;
    int widthInTiles, heightInTiles;
    MapLocation centerLocation;
    Position position;
};

struct MapView {
    MapViewProperties properties;
};

MapViewProperties ReadMapViewPropertiesFromJSON(const nlohmann::json& mapViewJSON, Game* game);

void CenterViewOnPlayer(MapView& view, const Map& map, MapLocation playerLocation);

#endif //DGTKPROJECT_VIEW_HPP
