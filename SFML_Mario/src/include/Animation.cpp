#include "Animation.h"

/*
	Stores all variables related to animation, with an update() function that provides logic to progress the animation
	CAnimation component stores Animation
	Very slight departure from only pure data in component, but very helpfull overall
*/

Animation::Animation() {}

Animation::Animation(const std::string& name, const sf::Texture& t)
	: Animation(name, t, 1, 0) {}

Animation::Animation(const std::string& name, const sf::Texture& t, size_t fc, size_t s)
	: m_name(name), m_sprite(t), m_frameCount(fc), m_currentFrame(0), m_speed(s)
{
	m_size = Vec2((float)(t.getSize().x / fc), (float)t.getSize().y);
	m_sprite.setOrigin(m_size.x / 2.0f, m_size.y / 2.0f);
	m_sprite.setTextureRect(sf::IntRect((int)std::floor(m_currentFrame * m_size.x), 0, (int)m_size.x, (int)m_size.y));
}

void Animation::update()
{
	// updates the animation to show the next frame, depending on its speed
	// animation loops when it reaches the end	
	if (m_frameCount == 1)
	{
		return;
	}
	m_currentFrame += 1;
	size_t frame = (m_currentFrame / m_speed) % m_frameCount;
	m_sprite.setTextureRect(sf::IntRect(frame * (int)m_size.x, 0, (int)m_size.x, (int)m_size.y));
}

const std::string& Animation::getName() const
{
	return m_name;
}

const Vec2& Animation::getSize() const
{
	return m_size;
}

sf::Sprite& Animation::getSprite()
{
	return m_sprite;
}

bool Animation::hasEnded() const
{
	if (m_frameCount == 1)
	{
		return false;
	}
	size_t frame = m_currentFrame / m_speed;
	return frame > m_frameCount;
}

void Animation::getDetails() const
{
	std::cout << "Name: " << m_name << ", Frame Count: " << m_frameCount << ", Current Frame: " 
		<< m_currentFrame << ", Speed: " << m_speed << "\n";
}