//
// Created by zeroc00l on 10/31/23.
//

#include <memory>
#include <vector>
#include "ui.hpp"
#include "map.hpp"

#ifndef DGTKPROJECT_SCENE_HPP
#define DGTKPROJECT_SCENE_HPP

struct Scene {
    std::vector<std::unique_ptr<uiObject>>      uiObjects;
    std::vector<uiObjectProperties>             uiProperties;
    Map                                         map;
};

#endif //DGTKPROJECT_SCENE_HPP
