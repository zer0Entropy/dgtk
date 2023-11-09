//
// Created by zeroc00l on 11/6/23.
//
#include "../include/decoration.hpp"
#include "../include/log.hpp"

DecorationProperties ReadDecorationPropertiesFromJSON(const nlohmann::json& jsonDoc) {
    DecorationProperties decProperties;

    for(auto decProperty = jsonDoc.begin(); decProperty != jsonDoc.end(); ++decProperty) {
        std::string_view key( decProperty.key() );
        auto value( decProperty.value() );

        if(key.compare(DecorationPropertyNames.at((int)DecorationPropertyID::ObjectID)) == 0) {
            decProperties.id = value.get<std::string>();
        } else if(key.compare(DecorationPropertyNames.at((int)DecorationPropertyID::DecorationType)) == 0) {
            std::string decTypeString = value.get<std::string>();
            if(decTypeString.compare("background") == 0) {
                decProperties.decType = DecorationType::Background;
            } else if(decTypeString.compare("frame") == 0) {
                decProperties.decType = DecorationType::Frame;
            } else if(decTypeString.compare("doodad") == 0) {
                decProperties.decType = DecorationType::Doodad;
            } else if(decTypeString.compare("text") == 0) {
                decProperties.decType = DecorationType::Text;
            }
        } else if(key.compare(DecorationPropertyNames.at((int)DecorationPropertyID::FontID)) == 0) {
            decProperties.fontID = value.get<std::string>();
        } else if(key.compare(DecorationPropertyNames.at((int)DecorationPropertyID::FontPath)) == 0) {
            decProperties.fontPath = value.get<std::string>();
        } else if(key.compare(DecorationPropertyNames.at((int)DecorationPropertyID::FontSize)) == 0) {
            decProperties.fontSize = value.get<int>();
        } else if(key.compare(DecorationPropertyNames.at((int)DecorationPropertyID::FontColor)) == 0) {
            auto fontColorJSON = value;
            if(fontColorJSON.is_string()) {
                std::string colorString(fontColorJSON.get<std::string>());
                if(colorString.compare("Black") == 0) {
                    decProperties.fontColor = sf::Color::Black;
                } else if(colorString.compare("White") == 0) {
                    decProperties.fontColor = sf::Color::White;
                } else if(colorString.compare("Red") == 0) {
                    decProperties.fontColor = sf::Color::Red;
                } else if(colorString.compare("Blue") == 0) {
                    decProperties.fontColor = sf::Color::Blue;
                } else if(colorString.compare("Green") == 0) {
                    decProperties.fontColor = sf::Color::Green;
                }
            } else {
                auto findR = fontColorJSON.find("r");
                auto findG = fontColorJSON.find("g");
                auto findB = fontColorJSON.find("b");
                auto findA = fontColorJSON.find("a");
                decProperties.fontColor = {
                        (sf::Uint8) findR->get<int>(),
                        (sf::Uint8) findG->get<int>(),
                        (sf::Uint8) findB->get<int>(),
                        (sf::Uint8) findA->get<int>()
                };
            }
        } else if(key == DecorationPropertyNames.at((int)DecorationPropertyID::OutlineColor)) {
            auto outlineColorJSON = value;
            if(outlineColorJSON.is_string()) {
                std::string colorString(outlineColorJSON.get<std::string>());
                if(colorString.compare("Black") == 0) {
                    decProperties.outlineColor = sf::Color::Black;
                } else if(colorString.compare("White") == 0) {
                    decProperties.outlineColor = sf::Color::White;
                } else if(colorString.compare("Red") == 0) {
                    decProperties.outlineColor = sf::Color::Red;
                } else if(colorString.compare("Blue") == 0) {
                    decProperties.outlineColor = sf::Color::Blue;
                } else if(colorString.compare("Green") == 0) {
                    decProperties.outlineColor = sf::Color::Green;
                } else {
                    auto findR = outlineColorJSON.find("r");
                    auto findG = outlineColorJSON.find("g");
                    auto findB = outlineColorJSON.find("b");
                    auto findA = outlineColorJSON.find("a");
                    decProperties.outlineColor = {
                            (sf::Uint8) findR->get<int>(),
                            (sf::Uint8) findG->get<int>(),
                            (sf::Uint8) findB->get<int>(),
                            (sf::Uint8) findA->get<int>()
                    };
                }
            }
        } else if(key == DecorationPropertyNames.at((int)DecorationPropertyID::OutlineThickness)) {
            decProperties.outlineThickness = value.get<float>();
        } else if(key == DecorationPropertyNames.at((int)DecorationPropertyID::TextContents)) {
            decProperties.contents = value.get<std::string>();
        }
    } // for each decProperty

return decProperties;
}

nlohmann::json WriteDecorationPropertiesToJSON(const DecorationProperties& decProperties) {
    nlohmann::json decorationJSON;
    decorationJSON[DecorationPropertyNames.at((int)DecorationPropertyID::DecorationType)] = decProperties.decType;
    decorationJSON[DecorationPropertyNames.at((int)DecorationPropertyID::FontID)] = decProperties.fontID;
    decorationJSON[DecorationPropertyNames.at((int)DecorationPropertyID::FontPath)] = decProperties.fontPath;
    decorationJSON[DecorationPropertyNames.at((int)DecorationPropertyID::FontSize)] = decProperties.fontSize;
    decorationJSON[DecorationPropertyNames.at((int)DecorationPropertyID::FontColor)] = nlohmann::json{
            {"r", decProperties.fontColor.r},
            {"g", decProperties.fontColor.g},
            {"b", decProperties.fontColor.b},
            {"a", decProperties.fontColor.a}
    };
    decorationJSON[DecorationPropertyNames.at((int)DecorationPropertyID::OutlineColor)] = nlohmann::json{
            {"r", decProperties.outlineColor.r},
            {"g", decProperties.outlineColor.g},
            {"b", decProperties.outlineColor.b},
            {"a", decProperties.outlineColor.a}
    };
    decorationJSON[DecorationPropertyNames.at((int)DecorationPropertyID::OutlineThickness)] = decProperties.outlineThickness;
    decorationJSON[DecorationPropertyNames.at((int)DecorationPropertyID::TextContents)] = decProperties.contents;
    return decorationJSON;
}
