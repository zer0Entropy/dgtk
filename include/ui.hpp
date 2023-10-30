//
// Created by zeroc00l on 10/30/23.
//

#include "id.hpp"

#ifndef DGTKPROJECT_UI_HPP
#define DGTKPROJECT_UI_HPP

enum class uiObjectType {
    Decoration,
    Toolbar,
    Scrollbar
};

struct uiObject {
    UniqueID        id;
    uiObjectType    type;
    explicit uiObject(UniqueID assignID, uiObjectType assignType):
        id(assignID), type(assignType) {

    }
};

#endif //DGTKPROJECT_UI_HPP
