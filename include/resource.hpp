//
// Created by zeroc00l on 10/30/23.
//

#include <map>
#include <memory>
#include <string>
#include <SFML/Graphics/Font.hpp>
#include <SFML/Graphics/Texture.hpp>
#include <SFML/Audio/SoundBuffer.hpp>
#include <SFML/Audio/Music.hpp>
#include "id.hpp"

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

class ResourceManager {
public:
    bool LoadResource(UniqueID id, ResourceType type, std::string_view path);
    ResourceHandle* GetResourceHandle(UniqueID id);
    sf::Texture* GetTexture(UniqueID id);
    sf::Font* GetFont(UniqueID id);
    sf::SoundBuffer* GetSound(UniqueID id);
    sf::Music* GetMusic(UniqueID id);
private:
    std::unique_ptr<sf::Texture> LoadTexture(std::string_view path);
    std::unique_ptr<sf::Font> LoadFont(std::string_view path);
    std::unique_ptr<sf::SoundBuffer> LoadSound(std::string_view path);
    std::unique_ptr<sf::Music> LoadMusic(std::string_view path);

    std::map<UniqueID, std::unique_ptr<ResourceHandle>>     resourceMap;
    std::map<UniqueID, std::unique_ptr<sf::Texture>>        textureMap;
    std::map<UniqueID, std::unique_ptr<sf::Font>>           fontMap;
    std::map<UniqueID, std::unique_ptr<sf::SoundBuffer>>    soundMap;
    std::map<UniqueID, std::unique_ptr<sf::Music>>          musicMap;
};

#endif //DGTKPROJECT_RESOURCE_HPP
