//
// Created by zeroc00l on 11/10/23.
//

#include "../include/action.hpp"
#include "../include/game.hpp"
#include "../include/log.hpp"

Action ReadActionFromJSON(const nlohmann::json& actionJSON, Game *game) {
    Action action;
    auto findID = actionJSON.find("action_id");
    auto findType = actionJSON.find("action_type");
    auto findTrigger = actionJSON.find("action_trigger");
    auto findTriggerKey = actionJSON.find("trigger_key");
    auto findActorID = actionJSON.find("actor_id");
    auto findTargetID = actionJSON.find("target_id");
    auto findTargetDirection = actionJSON.find("target_direction");

    if(findID != actionJSON.end()) {
        action.id = findID->get<std::string>();
    }
    if(findType != actionJSON.end()) {
        std::string typeString = findType->get<std::string>();
        if(typeString.compare(ActionTypeNames.at((int)ActionType::None)) == 0) {
            action.type = ActionType::None;
        } else if(typeString.compare(ActionTypeNames.at((int)ActionType::TransitionToScene)) == 0) {
            action.type = ActionType::TransitionToScene;
        } else if(typeString.compare(ActionTypeNames.at((int)ActionType::MoveCreature)) == 0) {
            action.type = ActionType::MoveCreature;
        }
    }
    if(findTrigger != actionJSON.end()) {
        std::string triggerString = findTrigger->get<std::string>();
        if(triggerString.compare(ActionTriggerNames.at((int)ActionTrigger::None)) == 0) {
            action.trigger = ActionTrigger::None;
        } else if(triggerString.compare(ActionTriggerNames.at((int)ActionTrigger::OnKeyPress)) == 0) {
            action.trigger = ActionTrigger::OnKeyPress;
        }
    }
    if(findTriggerKey != actionJSON.end()) {
        std::string triggerKeyString = findTriggerKey->get<std::string>();
        auto keyMapping = KeyIdentifiers.find(triggerKeyString);
        if(keyMapping != KeyIdentifiers.end()) {
            action.triggerKey = keyMapping->second;
        }
    }
    if(findActorID != actionJSON.end()) {
        action.actorID = findActorID->get<std::string>();
    }
    if(findTargetID != actionJSON.end()) {
        action.targetID = findTargetID->get<std::string>();
    }
    if(findTargetDirection != actionJSON.end()) {
        std::string directionString = findTargetDirection->get<std::string>();
        action.targetDirection = Direction::None;
        if(directionString.compare("up") == 0) {
            action.targetDirection = Direction::Up;
        } else if(directionString.compare("down") == 0) {
            action.targetDirection = Direction::Down;
        } else if(directionString.compare("left") == 0) {
            action.targetDirection = Direction::Left;
        } else if(directionString.compare("right") == 0) {
            action.targetDirection = Direction::Right;
        }
    }

    return action;
}

nlohmann::json WriteActionToJSON(const Action& action) {
    nlohmann::json actionJSON;
    return actionJSON;
}

