#pragma once

#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <unordered_map>

constexpr int TILE_SIZE = 16;

class TextureManager
{
    std::unordered_map<std::string, sf::Texture> m_allTextureMap;

public:
    TextureManager();
};