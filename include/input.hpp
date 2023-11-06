//
// Created by zeroc00l on 10/31/23.
//

#include <SFML/Window/Window.hpp>
#include "../include/display.hpp"

#ifndef DGTKPROJECT_INPUT_HPP
#define DGTKPROJECT_INPUT_HPP

#include "system.hpp"

enum class ListenerType {
    KeyPressListener,
    MouseMoveListener,
    MouseClickListener,
    TotalNumListenerTypes
};

class Game;

class InputListener {
public:
    InputListener(Game* gamePtr): game(gamePtr) {}
    virtual ~InputListener() {}
    virtual void ReceiveInput(const sf::Event& event) = 0;
protected:
    Game* game;
};

class InputSystem: public System {
public:
    explicit InputSystem(DisplaySystem* displaySysPtr);
    InputSystem(const InputSystem& copy) = delete;
    ~InputSystem();

    void Init();
    void Update();
    void Shutdown();

    void AddListener(InputListener* listener, ListenerType type);
    void RemoveListener(InputListener* listener, ListenerType type);
private:
    DisplaySystem* displaySystem;
    std::vector<InputListener*>         listeners[(int)ListenerType::TotalNumListenerTypes];
};

#endif //DGTKPROJECT_INPUT_HPP
