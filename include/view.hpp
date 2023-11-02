//
// Created by zeroc00l on 11/1/23.
//

#include "map.hpp"

#ifndef DGTKPROJECT_VIEW_HPP
#define DGTKPROJECT_VIEW_HPP

struct MapView {
    int widthInPixels, heightInPixels;
    int widthInTiles, heightInTiles;
    MapLocation centerLocation;
};

void CenterViewOnPlayer(MapView& view, const Map& map, MapLocation playerLocation);


#endif //DGTKPROJECT_VIEW_HPP
