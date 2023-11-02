//
// Created by zeroc00l on 11/1/23.
//

#include "creature.hpp"
#include "input.hpp"

#ifndef DGTKPROJECT_PLAYER_HPP
#define DGTKPROJECT_PLAYER_HPP

#include <memory>
#include <string>

enum class Direction { None = 0, Up, Down, Left, Right };

struct Player {
    std::unique_ptr<Creature> character;
};

class Game;

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

#endif //DGTKPROJECT_PLAYER_HPP
