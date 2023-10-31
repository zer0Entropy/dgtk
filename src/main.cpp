#include <filesystem>
#include <SFML/Graphics.hpp>
#include "../include/display.hpp"
#include "../include/input.hpp"
#include "../include/log.hpp"
#include "../include/resource.hpp"
#include "../include/decoration.hpp"

#include "../include/game.hpp"

bool CreateWindowFrame(DisplaySystem& display, ResourceSystem& resourceSystem);
bool CreateGameTitle(DisplaySystem& display, ResourceSystem& resourceSystem);

int main()
{
    Game game(std::filesystem::current_path());
    game.Init();

    game.CreateWindowFrame();
    game.CreateGameTitle();

    auto displaySystem(game.GetDisplaySystem());
    auto window(displaySystem->GetWindow());

    while(window->isOpen()) {
        game.Update();
    }

    game.Shutdown();
}

