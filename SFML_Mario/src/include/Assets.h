#pragma once
#include <unordered_map>
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include "Animation.h"

class Assets
{
	std::unordered_map<std::string, sf::Texture>		m_textureMap;
	std::unordered_map<std::string, Animation>			m_animationMap;
	std::unordered_map<std::string, sf::SoundBuffer>					m_SoundsMap;
	std::unordered_map<std::string, sf::Font>			m_fontMap;

	void addTexture(const std::string& name, const std::string& path);
	void addAnimation(const std::string& name, Animation& anim);
	void addFont(const std::string& name, const std::string& path);
	void addSound(const std::string& name, const std::string& path);

public:
	void loadFromFile(const std::string& path);

	const sf::Texture& getTexture(const std::string& name) const;
	const Animation& getAnimation(const std::string& name) const;
	const sf::Font& getFont(const std::string& name) const;
	const sf::SoundBuffer& getSound(const std::string& name) const;
};