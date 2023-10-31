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
    } // for(event)
}

void InputSystem::Shutdown() {

}

