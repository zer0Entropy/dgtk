//
// Created by zeroc00l on 10/30/23.
//

#include <vector>
#include "resource.hpp"

#ifndef DGTKPROJECT_UI_HPP
#define DGTKPROJECT_UI_HPP

enum class uiObjectType {
    Decoration,
    Toolbar,
    Scrollbar
};

struct uiObjectProperties {
    uiObjectType    uiType;
    TextureSource   textureSource;
    Alignment       align;
    Position        origin;
    Position        relativePosition;
    Position        position;
};

struct uiObject {
    UniqueID            id;
    uiObjectProperties  uiProperties;
    std::vector<uiObject*> children;

    explicit uiObject(UniqueID assignID, uiObjectType assignType):
        id(assignID) {
        uiProperties.uiType = assignType;
    }
};

#endif //DGTKPROJECT_UI_HPP
