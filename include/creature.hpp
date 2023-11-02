//
// Created by zeroc00l on 11/1/23.
//

#include <string>
#include <SFML/Graphics/Texture.hpp>
#include <SFML/Graphics/Sprite.hpp>
#include "position.hpp"
#include "location.hpp"

#ifndef DGTKPROJECT_CREATURE_HPP
#define DGTKPROJECT_CREATURE_HPP

enum class CreatureType {
    Human
};

struct Creature {
    CreatureType type;
    std::string name;
    MapLocation location;
    Position position;
    sf::Texture* texture;
    sf::Sprite* sprite;
};

#endif //DGTKPROJECT_CREATURE_HPP
