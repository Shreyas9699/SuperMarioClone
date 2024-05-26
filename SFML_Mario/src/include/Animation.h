#pragma once
#include <string>
#include <SFML/Graphics.hpp>
#include "Vec2.h"

class Animation
{
	sf::Sprite		m_sprite;
	size_t			m_frameCount	= 1;		// total number of frames of animation
	size_t			m_currentFrame	= 0;		// the current frame of animation being played	
	size_t			m_speed			= 0;		// the speed to play this animation
	Vec2			m_size			= { 1, 1 };	// size of the animation frame
	std::string		m_name			= "none";

public:
	Animation();
	Animation(const std::string& name, const sf::Texture& t);
	Animation(const std::string& name, const sf::Texture& t, size_t fc, size_t s);
	
	void update();
	bool hasEnded() const;
	const std::string& getName() const;
	const Vec2& getSize() const;
	sf::Sprite& getSprite();
	void getDetails() const;
};