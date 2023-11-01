//
// Created by zeroc00l on 10/31/23.
//

#include <SFML/Window/Event.hpp>
#include "../include/input.hpp"

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
        for(int listenTypeIndex = 0; listenTypeIndex < (int)ListenerType::TotalNumListenerTypes; ++listenTypeIndex) {
            for (auto iterator = listeners[listenTypeIndex].begin();
                        iterator != listeners[listenTypeIndex].end(); ++iterator) {
                (*iterator)->ReceiveInput(event);
            } // for each listener
        } // for each ListenerType
    } // for(event)
}

void InputSystem::Shutdown() {

}

void InputSystem::AddListener(InputListener* listener, ListenerType type) {
    listeners[(int)type].push_back(listener);
}

void InputSystem::RemoveListener(InputListener* listener, ListenerType type) {
    bool found = false;
    std::vector<InputListener*>& listenList(listeners[(int)type]);
    for(auto iterator = listenList.begin(); !found && iterator != listenList.end(); ++iterator) {
        if((*iterator) == listener) {
            found = true;
            listenList.erase(iterator);
        }
    }
}
