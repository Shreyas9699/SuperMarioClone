#pragma once
#include <string>
#include <vector>
#include "Scene.h"

class Scene_Menu : public Scene
{
	std::string				 m_title;
	std::vector<std::string> m_menuStrings;
	std::vector<std::string> m_levelPaths;
	sf::Text				 m_menuText;
	sf::Text				 m_menuInstText;
	size_t					 m_selectedMenuIdx = 0;

	void init(const std::string& menuConfig = nullptr);
	void update() override;
	void onEnd() override;
	void sDoAction(const Action& action) override;
public:
	Scene_Menu(GameEngine* game = nullptr);
	void sRender() override;
};