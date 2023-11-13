//
// Created by zeroc00l on 11/13/23.
//

#include <deque>
#include "location.hpp"
#include "terrain.hpp"

#ifndef DGTKPROJECT_PATH_HPP
#define DGTKPROJECT_PATH_HPP

struct Path {
    std::deque<MapLocation>    steps;
};

#endif //DGTKPROJECT_PATH_HPP
