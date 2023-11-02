//
// Created by zeroc00l on 11/1/23.
//

#include <SFML/Window/Event.hpp>
#include "../include/player.hpp"
#include "../include/game.hpp"
#include "../include/view.hpp"

PlayerController::PlayerController(Player* playerPtr, Game* gamePtr):
    InputListener(), player(playerPtr), game(gamePtr) {

}

void PlayerController::ReceiveInput(const sf::Event& event) {
    MapLocation location(player->character->location);
    Direction moveDirection(Direction::None);
    if(event.type == sf::Event::EventType::KeyPressed) {
        if(event.key.code == sf::Keyboard::Up) {
            moveDirection = Direction::Up;
        } else if(event.key.code == sf::Keyboard::Down) {
            moveDirection = Direction::Down;
        } else if(event.key.code == sf::Keyboard::Left) {
            moveDirection = Direction::Left;
        } else if(event.key.code == sf::Keyboard::Right) {
            moveDirection = Direction::Right;
        }
        switch(moveDirection) {
            case Direction::Up:     location.y--;
                break;

            case Direction::Down:   location.y++;
                break;

            case Direction::Left:   location.x--;
                break;

            case Direction::Right:  location.x++;
                break;

            case Direction::None:   default:    break;
        }
        if(moveDirection != Direction::None) {
            bool success = game->MoveCreature(player->character.get(), location);
            if(success) {
                Scene* scene(game->GetCurrentScene());
                CenterViewOnPlayer(scene->view, *scene->map.get(), location);
            }
        }
    }
}

