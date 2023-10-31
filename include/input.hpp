//
// Created by zeroc00l on 10/31/23.
//

#include <SFML/Window/Window.hpp>

#ifndef DGTKPROJECT_INPUT_HPP
#define DGTKPROJECT_INPUT_HPP

#include "system.hpp"

class InputSystem: public System {
public:
    void Update();

private:
    sf::Window*     window;
};

#endif //DGTKPROJECT_INPUT_HPP
