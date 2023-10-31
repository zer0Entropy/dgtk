//
// Created by zeroc00l on 10/30/23.
//

#include "../include/resource.hpp"
#include "../include/decoration.hpp"

bool ResourceManager::LoadResource(UniqueID id, ResourceType type, std::string_view path) {
    bool success(false);

    if (type == ResourceType::Texture) {
        std::unique_ptr<sf::Texture> texture(LoadTexture(path));
        if (texture) {
            success = true;
            textureMap.insert(std::make_pair(id, std::move(texture)));
        }
    } else if (type == ResourceType::Font) {
        std::unique_ptr<sf::Font> font(LoadFont(path));
        if (font) {
            success = true;
            fontMap.insert(std::make_pair(id, std::move(font)));
        }
    } else if (type == ResourceType::Sound) {
        std::unique_ptr<sf::SoundBuffer> sound(LoadSound(path));
        if (sound) {
            success = true;
            soundMap.insert(std::make_pair(id, std::move(sound)));
        }
    } else if (type == ResourceType::Music) {
        std::unique_ptr<sf::Music> music(LoadMusic(path));
        if(music) {
            success = true;
            musicMap.insert(std::make_pair(id, std::move(music)));
    }
}

    ResourceHandle* resourceHandle = new ResourceHandle(id, type, path);
    resourceMap.insert(std::make_pair(id, resourceHandle));
    return success;
}

bool ResourceManager::LoadTexture(UniqueID id, std::string_view path, Position startPos, int width, int height) {
    bool success(false);
    std::unique_ptr<sf::Texture> texture(LoadTexture(path, startPos, width, height));
    if (texture) {
        success = true;
        textureMap.insert(std::make_pair(id, std::move(texture)));
    }
    return success;
}

bool ResourceManager::LoadFrameTextures(UniqueID id, std::string_view path, Position topLeftPos, int segmentWidth, int segmentHeight) {
    bool success(false);
    Position currentPos(topLeftPos);
    for(int segmentIndex = (int)FrameSegment::TopLeft; segmentIndex < (int)FrameSegment::TotalNumFrameSegments; segmentIndex++) {
        auto texture = std::make_unique<sf::Texture>();
        success = texture->loadFromFile(std::string(path), sf::IntRect(currentPos.x,
                                                                       currentPos.y,
                                                                       segmentWidth,
                                                                       segmentHeight));
        if(success) {
            UniqueID segmentID(id);
            segmentID.append(FrameSegmentNames.at(segmentIndex));
            textureMap.insert(std::make_pair(segmentID, std::move(texture)));
        }
    }
    return success;
}

ResourceHandle* ResourceManager::GetResourceHandle(UniqueID id) {
    ResourceHandle* resourceHandle(nullptr);
    auto iterator(resourceMap.find(id));
    if(iterator != resourceMap.end()) {
        resourceHandle = iterator->second.get();
    }
    return resourceHandle;
}

sf::Texture* ResourceManager::GetTexture(UniqueID id) {
    sf::Texture* texture(nullptr);
    auto iterator(textureMap.find(id));
    if(iterator != textureMap.end()) {
        texture = iterator->second.get();
    }
    return texture;
}

sf::Font* ResourceManager::GetFont(UniqueID id) {
    sf::Font* font(nullptr);
    auto iterator(fontMap.find(id));
    if(iterator != fontMap.end()) {
        font = iterator->second.get();
    }
    return font;
}

sf::SoundBuffer* ResourceManager::GetSound(UniqueID id) {
    sf::SoundBuffer* sound(nullptr);
    auto iterator(soundMap.find(id));
    if(iterator != soundMap.end()) {
        sound = iterator->second.get();
    }
    return sound;
}

sf::Music* ResourceManager::GetMusic(UniqueID id) {
    sf::Music* music(nullptr);
    auto iterator(musicMap.find(id));
    if(iterator != musicMap.end()) {
        music = iterator->second.get();
    }
    return music;
}

bool ResourceManager::SetWorkingDirectory(const std::filesystem::path& directory) {
    bool success(false);
    workingDirectory = directory;
    std::string cfgFilename("game.cfg");
    std::filesystem::path currentPath(workingDirectory);
    while(!success) {
        std::filesystem::path cfgFilePath(currentPath);
        cfgFilePath.append(cfgFilename);

        if (std::filesystem::exists(cfgFilePath)) {
            rootDirectory = currentPath;
            std::filesystem::path resourcePath(currentPath);
            resourcePath.append("resource");
            resourceDirectory = resourcePath;
            success = true;
        }
        else if(currentPath.parent_path() != currentPath){
            currentPath = currentPath.parent_path();
        }
        else {
            return false;
        }
    }
    return success;
}

const std::filesystem::path& ResourceManager::GetResourceDirectory() const {
    return resourceDirectory;
}

std::unique_ptr<sf::Texture> ResourceManager::LoadTexture(std::string_view path,
                                                          Position startPos,
                                                          int width,
                                                          int height) {
    auto texture = std::make_unique<sf::Texture>();
    bool success(false);

    if(width == 0 && height == 0) {
        success = texture->loadFromFile(std::string(path));
    }
    else {
        success = texture->loadFromFile(std::string(path), sf::IntRect(startPos.x,
                                                                       startPos.y,
                                                                       width,
                                                                       height));
    }
    if(!success) {
        texture.release();
        texture = nullptr;
    }
    return std::move(texture);
}

std::unique_ptr<sf::Font> ResourceManager::LoadFont(std::string_view path) {
    auto font = std::make_unique<sf::Font>();
    bool success = font->loadFromFile(std::string(path));
    if(!success) {
        font.release();
        font = nullptr;
    }
    return std::move(font);
}

std::unique_ptr<sf::SoundBuffer> ResourceManager::LoadSound(std::string_view path) {
    auto sound = std::make_unique<sf::SoundBuffer>();
    bool success = sound->loadFromFile(std::string(path));
    if(!success) {
        sound.release();
        sound = nullptr;
    }
    return std::move(sound);
}

std::unique_ptr<sf::Music> ResourceManager::LoadMusic(std::string_view path) {
    auto music = std::make_unique<sf::Music>();
    bool success = music->openFromFile(std::string(path));
    if(!success) {
        music.release();
        music = nullptr;
    }
    return std::move(music);
}
