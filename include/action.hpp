//
// Created by zeroc00l on 11/10/23.
//

#include <map>
#include <vector>
#include <SFML/Window/Keyboard.hpp>
#include <nlohmann/json.hpp>
#include "id.hpp"

#ifndef DGTKPROJECT_ACTION_HPP
#define DGTKPROJECT_ACTION_HPP

enum class ActionType {
    None = 0,
    TransitionToScene,
    MoveCreature,
    TotalNumActionTypes
};

const std::vector<std::string> ActionTypeNames = {
        {"none"},
        {"transition_to_scene"},
        {"move_creature"}
};

enum class ActionTrigger {
    None = 0,
    OnKeyPress,
    TotalNumActionTriggers
};

const std::vector<std::string> ActionTriggerNames = {
        {"none"},
        {"on_key_press"}
};

const std::map<std::string,sf::Keyboard::Key>   KeyIdentifiers = {
        {{"any_key"}, sf::Keyboard::Unknown},
        {{"up_arrow_key"}, sf::Keyboard::Up},
        {{"down_arrow_key"}, sf::Keyboard::Down},
        {{"left_arrow_key"}, sf::Keyboard::Left},
        {{"right_arrow_key"}, sf::Keyboard::Right}
};

struct Action {
    UniqueID        id;
    ActionType      type;
    ActionTrigger   trigger;
    sf::Keyboard::Key triggerKey;
    std::string     resourcePath;
    UniqueID        actorID;
    UniqueID        targetID;
};

class Game;

Action ReadActionFromJSON(const nlohmann::json& actionJSON, Game* game);
nlohmann::json WriteActionToJSON(const Action& action);

#endif //DGTKPROJECT_ACTION_HPP
