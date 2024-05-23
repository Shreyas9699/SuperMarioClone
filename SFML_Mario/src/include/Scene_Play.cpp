#include <fstream>
#include <random>
#include <thread>
#include "Scene_Play.h"
#include "GameEngine.h"
#include "Physics.h"
#include "Components.h"
#include "Action.h"
#include "Assets.h"

Scene_Play::Scene_Play(GameEngine* gameEngine, const std::string& levelPath)
	: Scene(gameEngine), m_levelPath(levelPath)
{
	init(m_levelPath);
}

void Scene_Play::init(const std::string& levelPath)
{
	std::cout << "Level Scene init started" << std::endl;
	registerAction(sf::Keyboard::P,		 "PAUSE");
	registerAction(sf::Keyboard::Escape, "QUIT");
	registerAction(sf::Keyboard::T,		 "TOGGLE_TEXTURE");			// toggle drawring (T)extures
	registerAction(sf::Keyboard::C,		 "TOGGLE_COLLISION");		// toggle drawring (T)extures
	registerAction(sf::Keyboard::G,		 "TOGGLE_GRID");			// toggle drawring (T)extures
	registerAction(sf::Keyboard::W,		 "UP");
	registerAction(sf::Keyboard::A,		 "LEFT");
	registerAction(sf::Keyboard::D,		 "RIGHT");
	registerAction(sf::Keyboard::Space,	 "FIRE");

	m_gridText.setCharacterSize(2);
	m_gridText.setFont(m_game->getAssets().getFont("dogica"));

	// Game Score
	m_scoreText.setCharacterSize(30);
	m_scoreText.setFont(m_game->getAssets().getFont("Mario"));
	m_scoreText.setFillColor(sf::Color::White);

	// Level Timre
	m_timerText.setCharacterSize(30);
	m_timerText.setFont(m_game->getAssets().getFont("Mario"));
	m_timerText.setFillColor(sf::Color::White);

	sf::View view = m_game->window().getView();
	view.setSize(width() / 4.0f, height() / 4.0f);
	m_game->window().setView(view);
	auto start = std::chrono::high_resolution_clock::now();
	loadLevel(levelPath);
	auto end = std::chrono::high_resolution_clock::now();
	std::cout << "Time taken to load Scene and its scene entities : " << std::chrono::duration_cast<std::chrono::milliseconds>(end - start) << std::endl;
}

Vec2 Scene_Play::gridToMidPixel(float gridX, float gridY, std::shared_ptr<Entity> entity) const
{
	/*
		This function takes in a grid (x,y) position and an Entity
		Return a Vec2 indicating where the CENTER position of the Entity
		Should be You must use the Entity's Animation size to position it correctly 
		The size of the grid width and height is stored in m_gridSize.x and m_gridSize.y 
		The up-left corner of the Animation should align with the bottom left of the grid cell
	*/
	float b1x = gridX * m_gridSize.x;
	float b1y = height() - (gridY * m_gridSize.y);
	Vec2 size = entity->getComponent<CAnimation>().animation.getSize();
	return Vec2(b1x + size.x / 2.0f, b1y - size.y / 2.0f);
}

void Scene_Play::loadLevel(const std::string& levelPath)
{
	// this creats new entity manager
	m_entityManager = EntityManager();

	// read in the level file and add the appropriate entities
	// user the PlayerConfig struct m_playerConfig to sotre player properties
	// this struct is defined at the top of Scene_Play.h

	std::ifstream file(levelPath);
	std::string text;

	while (file.good())
	{
		file >> text;
		if (text == "Tile")
		{
			std::string name;
			float x, y;
			file >> name >> x >> y;
			std::cout << "Creating Tile entity with name: '" << name << "' and pos (" << x << "," << y << ")\n";

			auto e = m_entityManager.addEntity(name);
			e->addComponents<CTransform>(gridToMidPixel(x, y, e));
			e->addComponents<CAnimation>(m_game->getAssets().getAnimation(name), true);
			(name != "Pole") ? e->addComponents<CBoundingBox>(m_game->getAssets().getAnimation(name).getSize()) :
				e->addComponents<CBoundingBox>(Vec2(0.0f, m_game->getAssets().getAnimation(name).getSize().y));
		}
		else if (text == "Dec")
		{
			std::string name;
			float x, y;
			file >> name >> x >> y;
			std::cout << "Creating Dec entity with name: '" << name << "' and pos (" << x << "," << y << ")\n";

			auto e = m_entityManager.addEntity(name);
			e->addComponents<CTransform>(gridToMidPixel(x, y, e));
			e->addComponents<CAnimation>(m_game->getAssets().getAnimation(name), false);
		}
		else if (text == "Player")
		{
			std::cout << "Creating Player entity \n";
			file >> m_playerConfig.X >> m_playerConfig.Y >> m_playerConfig.CX >> m_playerConfig.CY
				 >> m_playerConfig.SPEED >> m_playerConfig.MAXSPEED >> m_playerConfig.JUMP >> m_playerConfig.GRAVITY
				 >> m_playerConfig.WEAPON;
		}
		else if (text == "Goomba")
		{
			std::cout << "Creating Goomba Position vector \n";
			float x, y;
			while (file >> text && text != "END")
			{
				std::istringstream ss(text);
				if (ss >> x)
				{
					file >> y;
					m_goombaPositions.push_back(Vec2(x, y));
				}
			}
			std::cout << "Done Creating Goomba Position vector, total number of differen pos: " << m_goombaPositions.size() << std::endl;
		}
	}
	startTimer = std::chrono::high_resolution_clock::now();
	spawnPlayer();
}

void Scene_Play::spawnPlayer()
{
	if (m_player) { m_player->destroy(); }

	std::cout << "Creating Player entity\n";
	m_player = m_entityManager.addEntity("player");
	m_player->addComponents<CAnimation>(m_game->getAssets().getAnimation("MarioRun"), true);
	m_player->addComponents<CTransform>(gridToMidPixel(m_playerConfig.X, m_playerConfig.Y, m_player), Vec2(), Vec2(-1.0f, 1.0f), 0.0f);
	m_player->addComponents<CInput>();
	m_player->addComponents<CBoundingBox>(Vec2(m_playerConfig.CX, m_playerConfig.CY));
	m_player->addComponents<CGravity>(m_playerConfig.GRAVITY);
	std::cout << "Completed creating Player entity\n";
}

void Scene_Play::spawnBullet(std::shared_ptr<Entity>& e)
{
	// Spawn a bullet at the given entity, going in the direction the entity is facing
	Vec2 ePos = e->getComponent<CTransform>().pos;
	std::string name = "Bullet";
	(e->getComponent<CTransform>().scale.x < 0) ? ePos.x += 16.0f : ePos.x -= 16.0f;
	auto b = m_entityManager.addEntity(name);
	auto& bulletTransform = b->addComponents<CTransform>(ePos);
	b->addComponents<CAnimation>(m_game->getAssets().getAnimation(name), true);
	b->addComponents<CBoundingBox>(m_game->getAssets().getAnimation(name).getSize() / 2);
	b->addComponents<CGravity>(m_playerConfig.GRAVITY);
	bulletTransform.velocity.x = -1 * e->getComponent<CTransform>().scale.x * m_playerConfig.SPEED * 3;
	b->addComponents<CLifeSpan>(1.832);
	// flip the bullet texture
	if (e->getComponent<CTransform>().scale.x > 0)
	{
		b->getComponent<CTransform>().scale.x = -1.0f;
	}
}

void Scene_Play::spawnEnemy()
{
	sf::View currentView = m_game->window().getView();

	float viewL = currentView.getCenter().x - (currentView.getCenter().x / 2.0f);
	float viewR = currentView.getCenter().x + (currentView.getCenter().x / 2.0f);
	float viewT = currentView.getCenter().y - (currentView.getCenter().y / 2.0f);
	float viewB = currentView.getCenter().y + (currentView.getCenter().y / 2.0f);
	
	auto dummyGoomba = m_entityManager.addEntity("dummyGoomba");
	dummyGoomba->addComponents<CAnimation>(m_game->getAssets().getAnimation("Goomba"), false);

	auto it = m_goombaPositions.begin();
	while (it != m_goombaPositions.end())
	{
		Vec2 pixelPos = gridToMidPixel(it->x, it->y, dummyGoomba);

		if (pixelPos.x >= viewL && pixelPos.x <= viewR && pixelPos.y >= viewT && pixelPos.y <= viewB)
		{
			createGoomba(*it);
			it = m_goombaPositions.erase(it);
		}
		else
		{
			break;
		}
	}
	dummyGoomba->destroy();
}

void Scene_Play::createGoomba(const Vec2& position)
{
	std::cout << "Creating Goomba entity at  position " << position << "\n";
	auto goomba = m_entityManager.addEntity("Goomba");
	goomba->addComponents<CAnimation>(m_game->getAssets().getAnimation("Goomba"), true);
	goomba->addComponents<CTransform>(gridToMidPixel(position.x, position.y, goomba), Vec2(GoombaSPEED, 0.0f), Vec2(1.0f, 1.0f), 0.0f);
	goomba->addComponents<CBoundingBox>(m_game->getAssets().getAnimation("Goomba").getSize());
	goomba->addComponents<CGravity>(m_playerConfig.GRAVITY);
}

void Scene_Play::playerDead()
{
	auto pos = m_player->getComponent<CTransform>().pos;
	m_player->destroy();
	auto deadPlayer = m_entityManager.addEntity("MarioDead");
	deadPlayer->addComponents<CTransform>(pos, Vec2(0.0f, YSPEED), Vec2(-1.0f, 1.0f), 0.0f);
	deadPlayer->addComponents<CAnimation>(m_game->getAssets().getAnimation("MarioDead"), true);
	deadPlayer->addComponents<CLifeSpan>(1.5);
}

void Scene_Play::update()
{
	spawnEnemy();
	m_entityManager.update();

	m_StateChanged = false;
	if (!m_paused)
	{
		sMovement();
		sLifeSpan();
		sCollision();
		sAnimation();
	}
}

void Scene_Play::sMovement()
{
	// Player movement
	auto& input = m_player->getComponent<CInput>();
	auto& transform = m_player->getComponent<CTransform>();

	if (input.up && input.canJump)
	{
		transform.velocity.y -= m_playerConfig.JUMP;
		input.canJump = false;
	}

	float xSpeed = 0.0f;
	if (input.right)
	{
		xSpeed += m_playerConfig.SPEED;
		transform.scale.x = -1.0f;
	}
	if (input.left)
	{
		xSpeed -= m_playerConfig.SPEED;
		transform.scale.x = 1.0f;
	}
	transform.velocity.x = xSpeed;

	if (xSpeed == 0.0f)
	{
		if (m_player->getComponent<CState>().state != "MarioStand")
		{
			m_player->getComponent<CState>().state = "MarioStand";
			m_StateChanged = true;
		}
	}
	else
	{
		if (m_player->getComponent<CState>().state != "MarioRun")
		{
			m_player->getComponent<CState>().state = "MarioRun";
			m_StateChanged = true;
		}
	}

	auto& gravity = m_player->getComponent<CGravity>();
	transform.velocity.y += gravity.gravity;

	if (transform.velocity.y >= m_playerConfig.MAXSPEED)
	{
		transform.velocity.y = m_playerConfig.MAXSPEED;
	}

	transform.prevPos = transform.pos;
	transform.pos += transform.velocity;

	// Bulleet movement
	for (auto& e : m_entityManager.getEntities("Bullet"))
	{
		e->getComponent<CTransform>().pos += e->getComponent<CTransform>().velocity;
	}

	// Goomba movement
	for (auto& goomba : m_entityManager.getEntities("Goomba"))
	{
		auto& transform = goomba->getComponent<CTransform>();
		transform.velocity.y += goomba->getComponent<CGravity>().gravity;

		// Cap the vertical velocity
		if (transform.velocity.y >= m_playerConfig.MAXSPEED)
		{
			transform.velocity.y = m_playerConfig.MAXSPEED;
		}
		// Update the position based on velocity
		transform.prevPos = transform.pos;
		transform.pos += transform.velocity;
	}

	// Miscellaneous : Coin, Star, Mushroom
	for (const auto& tag : m_rewards) 
	{
		for (auto& e : m_entityManager.getEntities(tag)) 
		{
			if (e->getComponent<CAnimation>().animation.getName() == "Coin")
			{
				auto& transform = e->getComponent<CTransform>();
				if (transform.velocity.y == 0.0f)
				{
					transform.velocity.y = GoombaSPEED;
				}

				// Update the position based on velocity
				transform.velocity.y += e->getComponent<CGravity>().gravity / 2.0f;
				transform.prevPos = transform.pos;
				transform.pos += transform.velocity;
				continue;
			}

			auto& transform = e->getComponent<CTransform>();
			auto& state = e->getComponent<CState>();

			if (state.state == "Emerging")
			{
				if (transform.pos.y >= transform.initialPos.y - 16.5f)  // Adjust this value as needed
				{
					transform.pos.y -= 0.5f;
				}
				else
				{
					transform.pos.y = transform.initialPos.y - 16.5f;  // Ensure it exactly reaches the target height
					state.state = "Moving";  // Transition to Moving state
					if (!e->hasComponent<CBoundingBox>())
					{
						Vec2 BB = m_game->getAssets().getAnimation(e->getComponent<CAnimation>().animation.getName()).getSize();
						e->addComponents<CBoundingBox>(Vec2(BB.x - 1, BB.y));
					}
				}
			}
			else if (state.state == "Moving")
			{
				// Apply gravity
				transform.velocity.y += e->getComponent<CGravity>().gravity;

				// Ensure Mushroom moves left
				if (transform.velocity.x == 0.0f)
				{
					transform.velocity.x = -1 * (GoombaSPEED / 1.5f); // Set initial direction to move left
				}

				// Update the position based on velocity
				transform.prevPos = transform.pos;
				transform.pos += transform.velocity;
			}
		}
	}
}

void Scene_Play::sLifeSpan()
{
	// Check Lifespan of entities that have them, and destroy them id they go over
	for (auto& e : m_entityManager.getEntities())
	{
		if (e->hasComponent<CLifeSpan>())
		{
			if (e->getComponent<CLifeSpan>().lifeSpan < e->getComponent<CLifeSpan>().clock.getElapsedTime().asSeconds())
			{
				e->destroy();
			}
		}
	}
}

void Scene_Play::sCollision()
{
	// TODO: Implement bullet / tile collisions
	//       Destroy the tile if it has a Brick animation
	sf::Sound stomp;
	auto& pPos = m_player->getComponent<CTransform>().pos;
	bool hasCollision = false;

	// Player
	for (auto& e : m_entityManager.getEntities())
	{
		if (!e->hasComponent<CBoundingBox>() || !e->hasComponent<CTransform>() || e == m_player)
			continue;

		Vec2 overlap = Physics::GetOverlap(e, m_player);
		Vec2 prevOverlap = Physics::GetPreviousOverlap(e, m_player);

		if (!(overlap.x < 0.0f || overlap.y < 0.0f))
		{
			hasCollision = true;
			auto& qPos = e->getComponent<CTransform>().pos;
			auto& pPrevPos = m_player->getComponent<CTransform>().prevPos;
			const std::string& name = e->getComponent<CAnimation>().animation.getName();

			// player and Enemy collosion
			if (name == "Piranha")
			{
				m_player->destroy();
				spawnPlayer();
			}

			// player and castle collosion
			if (name == "Pole" || name == "Flag" || name == "FlagTop")
			{
				m_player->destroy();
				onLevelEnd();
			}

			// player reward collosion
			if (name == "Star")
			{
				// should make them invincible
				e->destroy();
			}

			if (name == "MushroomR")
			{
				// Turn into Super Mario
				e->destroy();
			}

			if (name == "MushroomG")
			{
				// Give Extra lives
				e->destroy();
			}

			if (prevOverlap.y > 0.0f)
			{
				if (name == "Goomba")
				{
					playerDead();
					spawnPlayer();
				}
				pPos.x += pPrevPos.x < qPos.x ? -overlap.x : overlap.x;
			}

			if (prevOverlap.x > 0.0f)
			{
				if (pPrevPos.y > qPos.y)
				{
					pPos.y += overlap.y;
					const std::string& name = e->getComponent<CAnimation>().animation.getName();
					if (name == "Question")
					{
						Vec2 pos = e->getComponent<CTransform>().pos;
						e->addComponents<CAnimation>(m_game->getAssets().getAnimation("Solid"), true);
						// Seed the random number generator
						std::random_device rd;
						std::mt19937 gen(rd());

						// Choose a random index from 0 to vec.size() - 1 and each index has its weights
						std::discrete_distribution<> dis(m_weights.begin(), m_weights.end());
						int randomIndex = dis(gen);

						// Get the random element from the vector
						std::string eName = m_rewards[randomIndex];
						//std::string eName = "MushroomG";
						auto reward = m_entityManager.addEntity(eName);
						if (eName != "Coin") 
						{
							Vec2 rewardSize = m_game->getAssets().getAnimation(eName).getSize();
							reward->addComponents<CTransform>(Vec2(pos.x, pos.y - rewardSize.y / 2));
							reward->getComponent<CTransform>().initialPos = pos;
							reward->addComponents<CState>("Emerging"); // Set initial state to Emerging for non-Coin rewards
						}
						else
						{
							reward->addComponents<CTransform>(Vec2(pos.x, pos.y - 16.5f));
						}
						reward->addComponents<CAnimation>(m_game->getAssets().getAnimation(eName), true);
						reward->addComponents<CGravity>(m_playerConfig.GRAVITY);
						if (eName == "Coin")
						{
							m_score += 100;
							reward->addComponents<CLifeSpan>(0.5f);
						}
						else if (eName == "Star")
						{
							reward->addComponents<CLifeSpan>(6.0f);
						}
					}
					else if (name == "Brick")
					{
						Vec2& pos = e->getComponent<CTransform>().pos;
						e->destroy();
						auto eExplosion = m_entityManager.addEntity("Explosion");
						eExplosion->addComponents<CTransform>(pos);
						eExplosion->addComponents<CAnimation>(m_game->getAssets().getAnimation("Explosion"), false);
						eExplosion->addComponents<CLifeSpan>(1.8);
					}
				}
				else
				{
					if (name == "Goomba")
					{
						Vec2& pos = e->getComponent<CTransform>().pos;
						e->destroy(); 
						//const sf::SoundBuffer& buffer = m_game->getAssets().getSound("StompGoomba");
						//std::cout << "Buffer Sample Count: " << buffer.getSampleCount() << std::endl;
						//stomp.setBuffer(buffer);
						//stomp.play();
						//if (stomp.getStatus() == sf::Sound::Playing) {
						//	std::cout << "Playing\n";
						//}
						//else {
						//	std::cout << "Sound Not Playing (After Playing). \n";  // Added message
						//}
						auto eGBDead = m_entityManager.addEntity("GoombaDead");
						eGBDead->addComponents<CTransform>(pos);
						eGBDead->addComponents<CAnimation>(m_game->getAssets().getAnimation("GoombaDead"), false);
						eGBDead->addComponents<CLifeSpan>(0.5f);
					}
					pPos.y -= overlap.y;
					m_player->getComponent<CInput>().canJump = true;
				}
				m_player->getComponent<CTransform>().velocity.y = 0.0f;
			}
		}
		if (!m_player->getComponent<CInput>().canJump && prevOverlap.y > 0.0f)
		{
			if (m_player->getComponent<CState>().state != "MarioAir")
			{
				m_player->getComponent<CState>().state = "MarioAir";
				m_StateChanged = true;
			}
		}
	}


	if (pPos.y > height())
	{
		m_player->destroy();
		spawnPlayer();
	}

	if (pPos.x < 0.0f)
	{
		pPos.x = 1.5f;
	}

	// Bullet
	for (auto& eb : m_entityManager.getEntities("Bullet"))
	{
		auto& bPos = eb->getComponent<CTransform>().pos;
		bool hasCollision = false;
		for (auto& e : m_entityManager.getEntities())
		{
			if (e == m_player || e == eb || !e->hasComponent<CBoundingBox>() || !e->hasComponent<CTransform>())
			{
				continue;
			}
			Vec2 overlap = Physics::GetOverlap(e, eb);
			Vec2 prevOverlap = Physics::GetPreviousOverlap(e, eb);

			if (!(overlap.x < 0.0f || overlap.y < 0.0f))
			{
				auto& qPos = e->getComponent<CTransform>().pos;
				auto& bPrevPos = eb->getComponent<CTransform>().prevPos;
				const std::string& name = e->getComponent<CAnimation>().animation.getName();
				if (name == "Brick")
					{
						Vec2& pos = e->getComponent<CTransform>().pos;
						e->destroy();
						eb->destroy();
						auto eExplosion = m_entityManager.addEntity("Explosion");
						eExplosion->addComponents<CTransform>(pos);
						eExplosion->addComponents<CAnimation>(m_game->getAssets().getAnimation("Explosion"), false);
						eExplosion->addComponents<CLifeSpan>(1.8);
					}
				else if (name == "Goomba")
					{
						e->destroy();
						eb->destroy();
					}
				else
					{
						eb->destroy();
					}
			}
		}

	}

	// Goomba
	for (auto& goomba : m_entityManager.getEntities("Goomba"))
	{
		auto& gPos = goomba->getComponent<CTransform>().pos;

		for (auto& e : m_entityManager.getEntities())
		{
			if (!e->hasComponent<CBoundingBox>() || !e->hasComponent<CTransform>() || e == goomba)
			{
				continue;
			}
			Vec2 overlap = Physics::GetOverlap(e, goomba);
			Vec2 prevOverlap = Physics::GetPreviousOverlap(e, goomba);

			if (!(overlap.x < 0.0f || overlap.y < 0.0f))
			{
				auto& ePos = e->getComponent<CTransform>().pos;
				auto& gPrevPos = goomba->getComponent<CTransform>().prevPos;
				const std::string& name = e->getComponent<CAnimation>().animation.getName();

				// Similar collision responses as the player
				if (prevOverlap.y > 0.0f)
				{
					gPos.x += gPrevPos.x < ePos.x ? -overlap.x : overlap.x;
					// Change direction upon horizontal collision
					goomba->getComponent<CTransform>().velocity.x *= -1;
				}
				if (prevOverlap.x > 0.0f)
				{
					if (gPrevPos.y > ePos.y)
					{
						gPos.y += overlap.y;
					}
					else
					{
						gPos.y -= overlap.y;
					}
					goomba->getComponent<CTransform>().velocity.y = 0.0f;
				}

				// Handle falling out of the scene
				if (gPos.y > height())
				{
					goomba->destroy();
				}
				if (gPos.x < 0.0f)
				{
					//gPos.x = 0.0f;
					//goomba->getComponent<CTransform>().velocity.x *= -1;
					goomba->destroy();
				}
			}
		}
	}


	// // Miscellaneous : Coin, Star, Mushroom
	for (auto& tag : m_rewards)
	{
		for (auto& eR : m_entityManager.getEntities(tag))
		{
			auto& eRPos = eR->getComponent<CTransform>().pos;
			const std::string& eRname = eR->getComponent<CAnimation>().animation.getName();

			for (auto& e : m_entityManager.getEntities())
			{
				if (!e->hasComponent<CBoundingBox>() || !e->hasComponent<CTransform>() || e == eR || eRname == "Coin")
				{
					continue;
				}
				Vec2 overlap = Physics::GetOverlap(e, eR);
				Vec2 prevOverlap = Physics::GetPreviousOverlap(e, eR);

				if (!(overlap.x < 0.0f || overlap.y < 0.0f))
				{
					auto& ePos = e->getComponent<CTransform>().pos;
					auto& eRPrevPos = eR->getComponent<CTransform>().prevPos;

					// Similar collision responses as the player
					if (prevOverlap.y > 0.0f)
					{
						eRPos.x += eRPrevPos.x < ePos.x ? -overlap.x : overlap.x;
						// Change direction upon horizontal collision
						eR->getComponent<CTransform>().velocity.x *= -1;
					}
					if (prevOverlap.x > 0.0f)
					{
						if (eRPrevPos.y > ePos.y)
						{
							eRPos.y += overlap.y;
						}
						else
						{
							eRPos.y -= overlap.y;
						}
						eR->getComponent<CTransform>().velocity.y = 0.0f;
					}

					// Handle falling out of the scene
					if (eRPos.y > height() || eRPos.x < 0.0f)
					{
						eR->destroy();
					}
				}
			}
		}
	}
}

void Scene_Play::sAnimation()
{
	if (m_StateChanged)
	{
		m_player->addComponents<CAnimation>(m_game->getAssets().getAnimation(m_player->getComponent<CState>().state), true);
	}

	for (auto& e : m_entityManager.getEntities())
	{
		auto& anim = e->getComponent<CAnimation>();
		anim.animation.update();
		if (!anim.repeat && anim.animation.hasEnded())
		{
			e->destroy();
		}
	}
}

void Scene_Play::sRender()
{
	if (!m_paused)
	{
		m_game->window().clear(sf::Color(148, 148, 255));
	}
	else
	{
		m_game->window().clear(sf::Color(58, 58, 100));
	}

	// Setup UI view for score and timer rendering
	sf::View originalView = m_game->window().getView();
	sf::View uiView(sf::FloatRect(0, 0, m_game->window().getSize().x, m_game->window().getSize().y));
	m_game->window().setView(uiView);

	// Render score
	m_scoreText.setPosition(10, 10);
	m_scoreText.setString("Score: " + std::to_string(m_score)); 
	m_game->window().draw(m_scoreText);

	// Render timer
	std::string timeString = "Time: " + std::to_string((std::chrono::duration_cast<std::chrono::seconds>(std::chrono::high_resolution_clock::now() - startTimer)).count()) + "s";
	m_timerText.setString(timeString);
	sf::FloatRect textRect = m_timerText.getLocalBounds();
	m_timerText.setPosition(m_game->window().getSize().x - textRect.width - 10, 10);
	m_game->window().draw(m_timerText);

	// Restore original view
	m_game->window().setView(originalView);

	// Set the game view centered on the player
	auto& pPos = m_player->getComponent<CTransform>().pos;
	sf::View view = m_game->window().getView();
	float windowCenterX = std::max(view.getSize().x / 2.0f, pPos.x);
	view.setCenter(windowCenterX, m_game->window().getSize().y - view.getSize().y / 2.0f);
	m_game->window().setView(view);

	// Lists to store different types of entities
	std::vector<std::shared_ptr<Entity>> rewards;
	std::vector<std::shared_ptr<Entity>> blocks;
	std::vector<std::shared_ptr<Entity>> others;

	// Categorize entities
	for (auto& e : m_entityManager.getEntities())
	{
		if (e->getComponent<CAnimation>().animation.getName() == "player")
		{
			continue;
		}

		if (e->hasComponent<CState>() && e->getComponent<CState>().state == "Emerging")
		{
			rewards.push_back(e);
		}
		else if (e->getComponent<CAnimation>().animation.getName() == "Question")
		{
			blocks.push_back(e);
		}
		else
		{
			others.push_back(e);
		}
	}


	if (m_drawTexture)
	{
		for (auto& e : rewards)
		{
			auto& transform = e->getComponent<CTransform>();
			auto& animation = e->getComponent<CAnimation>().animation;
			animation.getSprite().setRotation(transform.angle);
			animation.getSprite().setPosition(transform.pos.x, transform.pos.y);
			animation.getSprite().setScale(transform.scale.x, transform.scale.y);
			m_game->window().draw(animation.getSprite());
		}

		// Render blocks
		for (auto& e : blocks)
		{
			auto& transform = e->getComponent<CTransform>();
			if (e->hasComponent<CAnimation>())
			{
				auto& animation = e->getComponent<CAnimation>().animation;
				animation.getSprite().setRotation(transform.angle);
				animation.getSprite().setPosition(transform.pos.x, transform.pos.y);
				animation.getSprite().setScale(transform.scale.x, transform.scale.y);
				m_game->window().draw(animation.getSprite());
			}
		}

		// Render other entities
		for (auto& e : others)
		{
			auto& transform = e->getComponent<CTransform>();
			if (e->hasComponent<CAnimation>())
			{
				auto& animation = e->getComponent<CAnimation>().animation;
				animation.getSprite().setRotation(transform.angle);
				animation.getSprite().setPosition(transform.pos.x, transform.pos.y);
				animation.getSprite().setScale(transform.scale.x, transform.scale.y);
				m_game->window().draw(animation.getSprite());
			}
		}

		// Draw player last to make sure it is not overlapped by other entities
		auto& transform = m_player->getComponent<CTransform>();
		auto& animation = m_player->getComponent<CAnimation>().animation;
		animation.getSprite().setRotation(transform.angle);
		animation.getSprite().setPosition(transform.pos.x, transform.pos.y);
		animation.getSprite().setScale(transform.scale.x, transform.scale.y);
		m_game->window().draw(animation.getSprite());
	}

	// Draw collision boxes if enabled
	if (m_drawCollision)
	{
		for (auto& e : m_entityManager.getEntities())
		{
			if (e->hasComponent<CBoundingBox>())
			{
				auto& name = e->getComponent<CAnimation>().animation.getName();
				auto& box = e->getComponent<CBoundingBox>();
				auto& transform = e->getComponent<CTransform>();
				sf::RectangleShape rect;
				rect.setSize(sf::Vector2f(box.size.x - 1, box.size.y - 1));
				rect.setPosition(transform.pos.x, transform.pos.y);
				rect.setOrigin(sf::Vector2f(box.halfSize.x, box.halfSize.y));
				rect.setFillColor(sf::Color::Transparent);
				rect.setOutlineColor(sf::Color::White);
				rect.setOutlineThickness(1.0f);
				m_game->window().draw(rect);
			}
		}
	}

	if (m_drawGrid) {
		for (float x = m_gridSize.x; x <= width(); x += m_gridSize.x)
		{
			drawLine(Vec2(x, 0), Vec2(x, height()));
		}

		for (float y = 0; y < height(); y += m_gridSize.y) 
		{
			drawLine(Vec2(0, height() - y), Vec2(width(), height() - y));
			/*for (float i = 0; i < width(); i += m_gridSize.x)
			{
				for (float j = 0; j < height(); j += m_gridSize.y)
				{
					m_gridText.setPosition(i, j);
					m_gridText.setString("(" + std::to_string((int)i / 64) + "," +
						std::to_string((int)j / 64) + ")");
					;
					m_game->window().draw(m_gridText);
				}
			}*/
		}
	}
}

void Scene_Play::sDoAction(const Action& action)
{
	auto& input = m_player->getComponent<CInput>();

	if (action.type() == "START")
	{
		if		(action.name() == "TOGGLE_TEXTURE")		{ m_drawTexture = !m_drawTexture; }
		else if (action.name() == "TOGGLE_COLLISION")	{ m_drawCollision = !m_drawCollision; }
		else if (action.name() == "TOGGLE_GRID")		{ m_drawGrid = !m_drawGrid; }
		else if (action.name() == "PAUSE")				{ m_paused = !m_paused; }
		else if (action.name() == "QUIT")				{ onEnd(); }
		else if (action.name() == "UP")					{ input.up = true; }
		else if (action.name() == "DOWN")				{ input.down = true; }
		else if (action.name() == "RIGHT")				{ input.right = true; }
		else if (action.name() == "LEFT")				{ input.left = true; }
		else if (action.name() == "FIRE" && !input.shoot)
		{
			input.shoot = true;
			spawnBullet(m_player); 
		}
	}
	else if (action.type() == "END")
	{
		if		(action.name() == "UP")		{ input.up = false; }
		else if (action.name() == "DOWN")	{ input.down = false; }
		else if (action.name() == "RIGHT")	{ input.right = false; }
		else if (action.name() == "LEFT")	{ input.left = false; }
		else if (action.name() == "FIRE")   { input.shoot = false; }
	}
}

void Scene_Play::onLevelEnd()
{
	m_game->changeScene("MENU", nullptr, true);
}

void Scene_Play::onEnd()
{
	m_game->changeScene("MENU", nullptr, true);
}

void Scene_Play::reloadLevel()
{
    // Clear all entities except the player
    m_entityManager.clearAllEntities();
    m_goombaPositions.clear();

    // Reload level from the beginning
    loadLevel(m_levelPath);
}