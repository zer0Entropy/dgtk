//
// Created by zeroc00l on 10/31/23.
//

#include <map>
#include <SFML/Window/Keyboard.hpp>
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

struct Action;

class KeyPressListener: public InputListener {
public:
    KeyPressListener(Game* gamePtr);
    KeyPressListener(const KeyPressListener& copy) = default;
    ~KeyPressListener() = default;

    void    ReceiveInput(const sf::Event& event);
    void    AddKeyMapping(sf::Keyboard::Key keyPress, Action action);
    void    RemoveKeyMapping(sf::Keyboard::Key keyPress);
    void    ChangeKeyMapping(sf::Keyboard::Key keyPress, Action action);
    Action* GetAction(sf::Keyboard::Key keyPress) const;
protected:
    std::map<sf::Keyboard::Key, Action> keyMap;
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
    const std::vector<InputListener*>& GetKeyPressListeners() const;
private:
    DisplaySystem* displaySystem;
    std::vector<InputListener*>         listeners[(int)ListenerType::TotalNumListenerTypes];
};

#endif //DGTKPROJECT_INPUT_HPP
