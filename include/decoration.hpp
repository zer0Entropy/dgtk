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
    Doodad,
    Text
};

struct DecorationProperties {
    DecorationType      decType;
    UniqueID            fontID;
    std::string         fontPath;
    int                 fontSize;
    sf::Color           fontColor;
    sf::Color           fillColor;
    sf::Color           outlineColor;
    float               outlineThickness;
    std::string         contents;
};

struct Decoration: public uiObject {
    DecorationProperties    decProperties;
    sf::Texture*            texture;
    sf::Font*               font;

    explicit Decoration(UniqueID assignID, DecorationType assignType):
        uiObject(assignID, uiObjectType::Decoration) {
        decProperties.decType = assignType;
    }
};

enum class FrameSegment {
    TopLeft = 0, TopMid, TopRight,
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
