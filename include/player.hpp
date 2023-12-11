//
// Created by zeroc00l on 11/1/23.
//

#include "creature.hpp"
#include "input.hpp"

#ifndef DGTKPROJECT_PLAYER_HPP
#define DGTKPROJECT_PLAYER_HPP

#include <memory>
#include <string>
#include "location.hpp"

class MapArea;

struct Player {
    std::unique_ptr<Creature> character;
    MapArea* currentArea;
};

class Game;
/*
class PlayerController: public InputListener {
public:
    PlayerController(Player* playerPtr, Game* gamePtr);
    PlayerController(const PlayerController& copy) = default;
    ~PlayerController() = default;

    void ReceiveInput(const sf::Event &event) override;
private:
    Player* player;
    Game* game;
};
*/
#endif //DGTKPROJECT_PLAYER_HPP
