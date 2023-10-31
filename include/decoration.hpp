//
// Created by zeroc00l on 10/30/23.
//

#include <SFML/Graphics/Texture.hpp>
#include "ui.hpp"

#ifndef DGTKPROJECT_DECORATION_HPP
#define DGTKPROJECT_DECORATION_HPP

enum class DecorationType {
    Background,
    Frame,
    Doodad
};

struct Decoration: public uiObject {
    DecorationType      type;
    sf::Texture*        texture;

    explicit Decoration(UniqueID assignID, DecorationType assignType):
        uiObject(assignID, uiObjectType::Decoration), type(assignType) {

    }
};

enum class FrameSegment {
    TopLeft, TopMid, TopRight,
    MidLeft, Middle, MidRight,
    BottomLeft, BottomMid, BottomRight,
    TotalNumFrameSegments
};

static std::vector<std::string> FrameSegmentNames{
    "TopLeft", "TopMiddle", "TopRight",
    "MidLeft", "Middle", "MidRight",
    "BottomLeft", "BottomMiddle", "BottomRight"
};

#endif //DGTKPROJECT_DECORATION_HPP
