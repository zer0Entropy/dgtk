//
// Created by zeroc00l on 10/30/23.
//

#include <filesystem>
#include <map>
#include <memory>
#include <string>
#include <SFML/Graphics/Font.hpp>
#include <SFML/Graphics/Texture.hpp>
#include <SFML/Audio/SoundBuffer.hpp>
#include <SFML/Audio/Music.hpp>
#include "id.hpp"
#include "position.hpp"
#include "system.hpp"

#ifndef DGTKPROJECT_RESOURCE_HPP
#define DGTKPROJECT_RESOURCE_HPP

enum class ResourceType {
    ConfigFile,
    Texture,
    Font,
    Sound,
    Music
};

struct ResourceHandle {
    UniqueID            id;
    ResourceType        type;
    std::string         path;

    explicit ResourceHandle(UniqueID assignID, ResourceType assignType, std::string_view assignPath):
        id(assignID), type(assignType), path(assignPath) {

    }
};

class ResourceSystem: public System {
public:
    explicit ResourceSystem(const std::filesystem::path& directory);
    ResourceSystem(const ResourceSystem& copy) = delete;
    ~ResourceSystem();

    void Init();
    void Update();
    void Shutdown();

    bool LoadResource(UniqueID id, ResourceType type, std::string_view path);
    bool LoadTexture(UniqueID id, std::string_view path, Position startPos, int width, int height);
    bool LoadFrameTextures(UniqueID id, std::string_view path, Position topLeftPos, int segmentWidth, int segmentHeight);

    ResourceHandle* GetResourceHandle(UniqueID id);
    sf::Texture* GetTexture(UniqueID id);
    sf::Font* GetFont(UniqueID id);
    sf::SoundBuffer* GetSound(UniqueID id);
    sf::Music* GetMusic(UniqueID id);

    bool SetWorkingDirectory(const std::filesystem::path& directory);
    const std::filesystem::path& GetResourceDirectory() const;
private:
    std::unique_ptr<sf::Texture> LoadTexture(std::string_view path,
                                             Position startPos = {0, 0},
                                             int width = 0,
                                             int height = 0);
    std::unique_ptr<sf::Font> LoadFont(std::string_view path);
    std::unique_ptr<sf::SoundBuffer> LoadSound(std::string_view path);
    std::unique_ptr<sf::Music> LoadMusic(std::string_view path);

    std::map<UniqueID, std::unique_ptr<ResourceHandle>>     resourceMap;
    std::map<UniqueID, std::unique_ptr<sf::Texture>>        textureMap;
    std::map<UniqueID, std::unique_ptr<sf::Font>>           fontMap;
    std::map<UniqueID, std::unique_ptr<sf::SoundBuffer>>    soundMap;
    std::map<UniqueID, std::unique_ptr<sf::Music>>          musicMap;

    std::filesystem::path                                   workingDirectory;
    std::filesystem::path                                   rootDirectory;
    std::filesystem::path                                   resourceDirectory;
};

#endif //DGTKPROJECT_RESOURCE_HPP
