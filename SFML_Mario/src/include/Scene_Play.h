#pragma once
#include <string>
#include <memory>
#include <chrono>
#include <thread>
#include <queue>
#include <SFML/Audio.hpp>
#include "Entity.h"
#include "Scene.h"

class Scene_Play : public Scene
{
	struct PlayerConfig {
		float X, Y, CX, CY, SPEED, MAXSPEED, JUMP, GRAVITY;
		std::string WEAPON;
	};

	struct Vec2Comparator {
		bool operator()(const Vec2& lhs, const Vec2& rhs) const {
			return lhs.x > rhs.x; // Descending order based on x value
		}
	};

	const float GoombaSPEED = -1.0f;
	const float YSPEED = 1.0f;
	const float shellSpeed = 2.0f;
	float TurtlePrevVel, TurtlePrevScale;

	std::shared_ptr<Entity>	m_player;
	std::string				m_levelPath;
	PlayerConfig			m_playerConfig;
	bool					m_drawTexture		= true;
	bool					m_drawCollision	    = false;
	bool					m_drawGrid			= false;
	bool					m_StateChanged		= false;
	bool					m_reloadingLevel    = false;
	bool					m_isSuperMario      = false;
	const Vec2				m_gridSize	= { 16.0f, 16 };
	sf::Text				m_gridText;
	long int				m_score = 0;
	sf::Text				m_scoreText;
	sf::Text				m_timerText;
	sf::Sound               m_sound;

	std::chrono::time_point<std::chrono::high_resolution_clock> startTimer;
	std::chrono::time_point<std::chrono::high_resolution_clock> pauseStartTime = std::chrono::high_resolution_clock::now();;
	
	std::priority_queue<Vec2, std::vector<Vec2>, Vec2Comparator> m_goombaPositions;
	std::priority_queue<Vec2, std::vector<Vec2>, Vec2Comparator> m_turtlePositions;
	std::vector<std::string> m_rewards		= { "Coin", "Star", "MushroomR", "MushroomG" };
	//Probability of selecting each reward is distributed as below
	// 50% for Coin, 25% for Star, and 12.5% each for MushroomR and MushroomG
	std::vector<int> m_weights = { 80, 15, 15, 10 };

	void init(const std::string& levelPath); 
	Vec2 gridToMidPixel(float gridX, float gridY, std::shared_ptr<Entity> entity) const;
	void loadLevel(const std::string& filePath);
	void spawnPlayer();
	void spawnEnemy();
	void createGoomba(const Vec2& position);
	void createTurtle(const Vec2& position);
	void playerDead();
	void spawnBullet(std::shared_ptr<Entity>& entity);
	void onLevelEnd();
	void onEnd();
	void reloadLevel();
	void setBoundingBox(std::shared_ptr<Entity>& entity, const std::string& animationName);

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