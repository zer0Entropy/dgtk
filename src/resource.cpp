//
// Created by zeroc00l on 10/30/23.
//

#include "../include/resource.hpp"

bool ResourceManager::LoadResource(UniqueID id, ResourceType type, std::string_view path) {
    bool success = false;

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

std::unique_ptr<sf::Texture> ResourceManager::LoadTexture(std::string_view path) {
    auto texture = std::make_unique<sf::Texture>();
    bool success = texture->loadFromFile(std::string(path));
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
