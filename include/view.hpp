//
// Created by zeroc00l on 11/1/23.
//

#include "map.hpp"
#include "ui.hpp"

#ifndef DGTKPROJECT_VIEW_HPP
#define DGTKPROJECT_VIEW_HPP

struct MapView {
    int widthInPixels, heightInPixels;
    int widthInTiles, heightInTiles;
    MapLocation centerLocation;
    Position position;
};

MapView CreateMapView(const nlohmann::json& mapViewJSON, Game* game);
void CenterViewOnPlayer(MapView& view, const Map& map, MapLocation playerLocation);

#endif //DGTKPROJECT_VIEW_HPP
