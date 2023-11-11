//
// Created by zeroc00l on 10/30/23.
//

#include <SFML/Graphics/Texture.hpp>
#include "ui.hpp"

#ifndef DGTKPROJECT_DECORATION_HPP
#define DGTKPROJECT_DECORATION_HPP

enum class DecorationPropertyID {
    ObjectID = 0,
    DecorationType,
    FrameWidth,
    FrameHeight,
    FontID,
    FontPath,
    FontSize,
    FontColor,
    OutlineColor,
    OutlineThickness,
    TextContents,
    TotalNumDecorationPropertyIDs
};

const std::vector<std::string> DecorationPropertyNames{
        {"object_id"},
        {"decoration_type"},
        {"frame_width"},
        {"frame_height"},
        {"font_id"},
        {"font_path"},
        {"font_size"},
        {"font_color"},
        {"outline_color"},
        {"outline_thickness"},
        {"contents"}
};

enum class DecorationType {
    Background,
    Frame,
    Doodad,
    Text
};

struct DecorationProperties {
    UniqueID            id;
    DecorationType      decType;
    int                 frameWidth;
    int                 frameHeight;
    UniqueID            fontID;
    std::string         fontPath;
    int                 fontSize;
    sf::Color           fontColor;
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

enum class FrameSegmentID {
    TopLeft = 0, TopMid, TopRight,
    MidLeft, Middle, MidRight,
    BottomLeft, BottomMid, BottomRight,
    TotalNumFrameSegmentIDs
};

static std::vector<std::string> FrameSegmentNames{
    "TopLeft", "TopMiddle", "TopRight",
    "MidLeft", "Middle", "MidRight",
    "BottomLeft", "BottomMiddle", "BottomRight"
};

class LogSystem;

DecorationProperties ReadDecorationPropertiesFromJSON(const nlohmann::json& jsonDoc, Game* game);
nlohmann::json WriteDecorationPropertiesToJSON(const DecorationProperties& decProperties);

#endif //DGTKPROJECT_DECORATION_HPP
