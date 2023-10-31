//
// Created by zeroc00l on 10/31/23.
//

#include <filesystem>
#include <memory>
#include "system.hpp"
#include "display.hpp"

#ifndef DGTKPROJECT_GAME_HPP
#define DGTKPROJECT_GAME_HPP

class LogSystem;
class ResourceSystem;
class InputSystem;

enum class GameStatus {
    Error = -1,
    MainMenu = 0,
    GamePlay = 1
};

class Game {
public:
    explicit Game(const std::filesystem::path& workingDir);
    Game(const Game& copy) = delete;
    ~Game();

    void    Init();
    void    Update();
    void    Shutdown();

    GameStatus  GetCurrentStatus() const;

    void        CreateWindowFrame();
    void        CreateGameTitle();

    DisplaySystem*      GetDisplaySystem() const;
    InputSystem*        GetInputSystem() const;
    LogSystem*          GetLogSystem() const;
    ResourceSystem*     GetResourceSystem() const;
private:
    bool        FindGameConfig();

    bool        LoadGameConfig();

    GameStatus                  status;

    std::unique_ptr<System>     systems[(int)SystemID::TotalNumSystems];

    std::filesystem::path       workingDirectory;
    std::filesystem::path       projectRoot;
    std::filesystem::path       resourceDirectory;
    std::filesystem::path       configDirectory;
    std::filesystem::path       configFilePath;

    DisplayConfig               displayConfig;

    static const std::string    configFilename;
};

#endif //DGTKPROJECT_GAME_HPP
