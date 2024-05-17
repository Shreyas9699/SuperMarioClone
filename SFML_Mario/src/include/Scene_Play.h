#pragma once
#include <string>
#include <memory>
#include <chrono>
#include "Entity.h"
#include "Scene.h"

class Scene_Play : public Scene
{
	struct PlayerConfig {
		float X, Y, CX, CY, SPEED, MAXSPEED, JUMP, GRAVITY;
		std::string WEAPON;
	};

	struct GoombaConfig {
		Vec2 pos;
	};

	const float GoombaSPEED = -1.0f;

	std::shared_ptr<Entity>	m_player;
	std::string				m_levelPath;
	PlayerConfig			m_playerConfig;
	bool					m_drawTexture		= true;
	bool					m_drawCollision	= false;
	bool					m_drawGrid			= false;
	bool					m_StateChanged		= false;
	const Vec2				m_gridSize	= { 16.0f, 16 };
	sf::Text				m_gridText;
	long int				m_score = 0;
	sf::Text				m_scoreText;
	sf::Text				m_timerText;
	std::chrono::time_point<std::chrono::high_resolution_clock> startTimer;
	std::chrono::time_point<std::chrono::high_resolution_clock> pauseStartTime = std::chrono::high_resolution_clock::now();;
	std::vector<GoombaConfig> m_goombaPos;
	std::vector<std::string> m_rewards = { "Coin", "Star", "MushroomR", "MushroomG" };
	bool Gbflg = false;

	void init(const std::string& levelPath); 
	Vec2 gridToMidPixel(float gridX, float gridY, std::shared_ptr<Entity> entity) const;
	void loadLevel(const std::string& filePath);
	void spawnPlayer();
	void spawnEnemy();
	void createGoomba();
	void spawnBullet(std::shared_ptr<Entity>& entity);
	void onLevelEnd();
	void onEnd();

	// Systems
	void sMovement();
	void sLifeSpan();
	void sCollision();
	void sAnimation();
	void sRender() override;
	void sDoAction(const Action& action) override;

public:
	Scene_Play(GameEngine* gameEngine, const std::string& levelPath);
	void update() override;
};