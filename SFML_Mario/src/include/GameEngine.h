#pragma once
#include <unordered_map>
#include <string>
#include "Scene.h"
#include "Assets.h"

typedef std::unordered_map<std::string, std::shared_ptr<Scene>> SceneMap;

class GameEngine
{
	sf::RenderWindow	m_window;
	Assets				m_assets;
	std::string			m_currentScene;
	SceneMap			m_sceneMap;
	size_t				m_simulationSpeed = 1;
	bool				m_running		  = true;

public:
	GameEngine(const std::string& path);
	void changeScene(std::string sceneName, std::shared_ptr<Scene> scene, bool endCurrentScene = false);
	void quit();
	void run();
	const Assets& getAssets();
	bool isRunning() const;
	sf::RenderWindow& window();

private:
	void init(const std::string& path);
	void update();
	void sUserInput();
	std::shared_ptr<Scene> getCurrentScene();
};