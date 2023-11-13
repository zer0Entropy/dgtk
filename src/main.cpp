#include <chrono>
#include <filesystem>
#include <SFML/Graphics.hpp>
#include "../include/display.hpp"
#include "../include/input.hpp"
#include "../include/log.hpp"
#include "../include/resource.hpp"
#include "../include/decoration.hpp"

#include "../include/game.hpp"

int main()
{
    typedef std::chrono::high_resolution_clock Clock;
    Clock::time_point startTime(Clock::now());
    Clock::duration elapsed(Clock::now() - startTime);
    Game game(std::filesystem::current_path(), elapsed.count());
    game.Init();

    ResourceSystem* resourceSystem(game.GetResourceSystem());

    UniqueID mainMenuID(SceneNames.at(SceneID::MainMenu));
    std::string scenePath(resourceSystem->GetScenePath(mainMenuID));
    Scene* mainMenu(resourceSystem->LoadScene(scenePath, game));
    game.TransitionTo(mainMenu);

    auto displaySystem(game.GetDisplaySystem());
    auto window(displaySystem->GetWindow());

    while(window->isOpen()) {
        game.Update();
    }

    game.Shutdown();
}