#include "Assets.h"
#include <iostream>
#include <fstream>

void Assets::loadFromFile(const std::string& path)
{
	std::ifstream file(path);
	if (file.is_open()) {
		std::cout << "File '" << path << "' was opened successfully." << std::endl;
	}
	else {
		std::cerr << "Error while opening file " << path  << std::endl;
		exit(-1);
	}
	std::string text;
	while (file.good())
	{
		file >> text;
		if (text == "Texture")
		{
			std::string name;
			std::string path;
			file >> name >> path;

			addTexture(name, path);
		}
		else if (text == "Animation")
		{
			std::string name;
			std::string textureName;
			size_t frameCount = 0, speed = 0;

			file >> name >> textureName >> frameCount >> speed;

			const sf::Texture& t = getTexture(textureName);
			Animation anim(name, t, frameCount, speed);
			addAnimation(name, anim);
		}
		else if (text == "Font")
		{
			std::string name;
			std::string path;

			file >> name >> path;

			addFont(name, path);
		}
		else if (text == "Sound")
		{
			std::string name;
			std::string path;

			file >> name >> path;
			std::cout << "Sound file name: " << name << " path: " << path << std::endl;

			addSound(name, path);
		}
	}
}

void Assets::addTexture(const std::string& name, const std::string& path)
{
	if (m_textureMap.find(name) != m_textureMap.end())
	{
		std::cout << "Texture '" << name << "' already exists" << std::endl;
		return;
	}
	sf::Texture t;
	if (!t.loadFromFile(path))
	{
		std::cerr << "Could not load texture '" << name << "' from path '" << path << "'" << std::endl;
		return;
	}
	else
	{
		m_textureMap[name] = t;
		std::cout << "Loaded texture '" << name << "' from path '" << path << "'" << std::endl;
	}
}

void Assets::addAnimation(const std::string& name, Animation& anim)
{
	if (m_animationMap.find(name) != m_animationMap.end())
	{
		std::cout << "Animation '" << name << "' already exists" << std::endl;
		return;
	}

	m_animationMap[name] = anim;
}

void Assets::addFont(const std::string& name, const std::string& path)
{
	if (m_fontMap.find(name) != m_fontMap.end())
	{
		std::cout << "Font '" << name << "' already exists" << std::endl;
		return;
	}
	if (!m_fontMap[name].loadFromFile(path))
	{
		std::cerr << "Could not load font '" << name << "' from path '" << path << "'" << std::endl;
	}
	else
	{
		std::cout << "Loaded Font '" << name << "' from path '" << path << "'" << std::endl;
	}
}

void Assets::addSound(const std::string& name, const std::string& path)
{
	if (m_SoundsMap.find(name) != m_SoundsMap.end())
	{
		std::cout << "Sound '" << name << "' already exists" << std::endl;
		return;
	}

	sf::SoundBuffer buffer;
	if (!buffer.loadFromFile(path))
	{
		std::cerr << "Could not load sound '" << name << "' from path '" << path << "'" << std::endl;
	}
	else
	{
		m_SoundsMap[name] = buffer;
		std::cout << "Loaded Sound '" << name << "' from path '" << path << "'" << std::endl;
	}
}

const sf::Texture& Assets::getTexture(const std::string& name) const
{
	auto it = m_textureMap.find(name);
	if (it != m_textureMap.end())
	{
		return it->second;
	}

	std::cerr << "Trying to use unknown texture called " << name << std::endl;
	exit(-1);
}

const Animation& Assets::getAnimation(const std::string& name) const
{
	auto it = m_animationMap.find(name);
	if (it != m_animationMap.end())
	{
		return it->second;
	}

	std::cerr << "Trying to use unknown animation called " << name << std::endl;
	exit(-1);
}

const sf::SoundBuffer& Assets::getSound(const std::string& name) const
{
	auto it = m_SoundsMap.find(name);
	if (it != m_SoundsMap.end())
	{
		return it->second;
	}

	std::cerr << "Trying to use unknown sound called " << name << std::endl;
	exit(-1);
}

const sf::Font& Assets::getFont(const std::string& name) const
{
	auto it = m_fontMap.find(name);
	if (it != m_fontMap.end())
	{
		return it->second;
	}

	std::cerr << "Trying to use unknown font called " << name << std::endl;
	exit(-1);
}