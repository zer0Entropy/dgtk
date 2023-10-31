//
// Created by zeroc00l on 10/31/23.
//

#include <SFML/Window/Window.hpp>
#include "../include/display.hpp"

#ifndef DGTKPROJECT_INPUT_HPP
#define DGTKPROJECT_INPUT_HPP

#include "system.hpp"

class InputSystem: public System {
public:
    explicit InputSystem(DisplaySystem* displaySysPtr);
    InputSystem(const InputSystem& copy) = delete;
    ~InputSystem();

    void Init();
    void Update();
    void Shutdown();
private:
    DisplaySystem* displaySystem;
};

#endif //DGTKPROJECT_INPUT_HPP
