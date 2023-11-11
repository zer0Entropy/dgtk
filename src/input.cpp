//
// Created by zeroc00l on 10/31/23.
//

#include <SFML/Window/Event.hpp>
#include "../include/input.hpp"
#include "../include/action.hpp"
#include "../include/game.hpp"

KeyPressListener::KeyPressListener(Game* gamePtr) : InputListener(gamePtr) {

}

void KeyPressListener::ReceiveInput(const sf::Event& event) {
    ResourceSystem* resourceSystem = game->GetResourceSystem();

    if(event.type == sf::Event::KeyPressed) {
        sf::Keyboard::Key keyPressed = event.key.code;
        Action* action = GetAction(keyPressed);
        Action* anyKeyAction = GetAction(sf::Keyboard::Key::Unknown);
        if(anyKeyAction) {
            switch (anyKeyAction->type) {
                case ActionType::None:
                    break;
                case ActionType::TransitionToScene: {
                    UniqueID sceneID = anyKeyAction->targetID;
                    std::string path = resourceSystem->GetScenePath(sceneID);
                    Scene* scene = resourceSystem->LoadScene(path, *game);
                    game->TransitionTo(scene);
                    break;
                }
                case ActionType::MoveCreature: {
                    UniqueID actorID = anyKeyAction->actorID;
                    Creature* creaturePtr = game->FindCreature(actorID);
                    game->MoveCreature(creaturePtr, anyKeyAction->targetLocation);
                    break;
                }
                case ActionType::TotalNumActionTypes:
                default:
                    break;
            } // switch(action->type)
        } // if(anyKeyAction)
        else if(action) {
            if(action->triggerKey == sf::Keyboard::Key::Unknown || keyPressed == action->triggerKey) {
                switch (action->type) {
                    case ActionType::None:
                        break;
                    case ActionType::TransitionToScene: {
                        UniqueID sceneID = action->targetID;
                        std::string path = resourceSystem->GetScenePath(sceneID);
                        Scene* scene = resourceSystem->LoadScene(path, *game);
                        game->TransitionTo(scene);
                        break;
                    }
                    case ActionType::MoveCreature: {
                        UniqueID actorID = action->actorID;
                        Creature* creaturePtr = game->FindCreature(actorID);
                        game->MoveCreature(creaturePtr, action->targetLocation);
                        break;
                    }
                    case ActionType::TotalNumActionTypes:
                    default:
                        break;
                } // switch(action->type)
            } // if(action->triggerKey == "any" || keyPressed == triggerKey)
        } // if(action)
    } // if(event.type == keyPressed)
}

void KeyPressListener::AddKeyMapping(sf::Keyboard::Key keyPress, Action action) {
    keyMap.insert(std::make_pair(keyPress, action));
}

void KeyPressListener::RemoveKeyMapping(sf::Keyboard::Key keyPress) {
    auto keyMapIter = keyMap.find(keyPress);
    if(keyMapIter != keyMap.end()) {
        keyMap.erase(keyMapIter);
    }
}

void KeyPressListener::ChangeKeyMapping(sf::Keyboard::Key keyPress, Action action) {
    auto keyMapIter = keyMap.find(keyPress);
    if(keyMapIter != keyMap.end()) {
        keyMapIter->second = action;
    }
}

Action* KeyPressListener::GetAction(sf::Keyboard::Key keyPress) const {
    Action* actionPtr(nullptr);
    auto keyMapIter = keyMap.find(keyPress);
    if(keyMapIter != keyMap.end()) {
        actionPtr = (Action*)&(keyMapIter->second);
    }
    return actionPtr;
}

InputSystem::InputSystem(DisplaySystem* displaySysPtr): System(SystemID::Input), displaySystem(displaySysPtr) {
}

InputSystem::~InputSystem() {

}

void InputSystem::Init() {

}

void InputSystem::Update() {
    sf::Window* window = displaySystem->GetWindow();
    for (auto event = sf::Event{}; window->isOpen() && window->pollEvent(event); ) {
        if (event.type == sf::Event::Closed) {
            displaySystem->CloseWindow();
        } // if(Event::Closed)
        for(int listenTypeIndex = 0; !interrupt && listenTypeIndex < (int)ListenerType::TotalNumListenerTypes; ++listenTypeIndex) {
            auto& listenerList(listeners[listenTypeIndex]);
            for(InputListener* listener: listenerList) {
                listener->ReceiveInput(event);
            } // for each listener
        } // for each ListenerType
    } // for(event)
    interrupt = false;
}

void InputSystem::Shutdown() {

}

void InputSystem::AddListener(InputListener* listener, ListenerType type) {
    listeners[(int)type].push_back(listener);
}

void InputSystem::RemoveListener(InputListener* listener, ListenerType type) {
    bool found = false;
    std::vector<InputListener*>& listenList(listeners[(int)type]);
    if(listenList.empty()) {return;}
    for(auto iterator = listenList.begin(); !found && iterator != listenList.end(); ++iterator) {
        if((*iterator) == listener) {
            found = true;
            listenList.erase(iterator);
        }
    }
}
