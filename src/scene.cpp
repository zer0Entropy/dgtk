//
// Created by zeroc00l on 11/1/23.
//
#include <SFML/Window/Event.hpp>
#include "../include/scene.hpp"
#include "../include/game.hpp"

GameplayTransition::GameplayTransition(Game* gamePtr): InputListener(), game(gamePtr) {

}

void GameplayTransition::ReceiveInput(const sf::Event& event) {
    if(event.type == sf::Event::EventType::KeyPressed) {
        Scene* gameplay(game->GenerateScene(GameStatus::GamePlay));
        game->TransitionTo(gameplay);
    }
}

