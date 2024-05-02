#include <fstream>
#include <random>
#include <chrono>
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

	m_scoreText.setCharacterSize(20);
	m_scoreText.setFont(m_game->getAssets().getFont("Pixel"));
	m_scoreText.setFillColor(sf::Color::White);
	m_scoreText.setString("Score: " + std::to_string(m_score));

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
	float x, y;
	x = (gridX * m_gridSize.x) + (entity->getComponent<CAnimation>().animation.getSize().x );
	y = height() - (gridY * m_gridSize.y) - (entity->getComponent<CAnimation>().animation.getSize().y);
	return Vec2(x, y);
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
			(name == "Castle") ? e->addComponents<CBoundingBox>(Vec2(0.0f, m_game->getAssets().getAnimation(name).getSize().y * 5.0f))
				: e->addComponents<CBoundingBox>(m_game->getAssets().getAnimation(name).getSize());
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
	}
	spawnPlayer();
}

void Scene_Play::spawnPlayer()
{
	std::cout << "Creating Player entity\n";
	m_player = m_entityManager.addEntity("player");
	m_player->addComponents<CAnimation>(m_game->getAssets().getAnimation("MarioRun"), true);
	m_player->addComponents<CTransform>(gridToMidPixel(m_playerConfig.X, m_playerConfig.Y, m_player), Vec2(), Vec2(-1.0f, 1.0f), 0.0f);
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
	std::cout << "Creating enemy\n";
	spawnPiranha();
	spawnOwl();
}

void Scene_Play::spawnPiranha() {
	std::cout << "Creating Piranha\n";
	std::vector<std::string> pipeTypes = { "PipeB", "PipeS", "PipeM" };
	std::vector<int> pipeXPositions;
	std::vector<int> pipeYPositions;

	// Find all pipe positions
	for (const auto& entity : m_entityManager.getEntities()) {
		if (entity->hasComponent<CTransform>() && entity->hasComponent<CBoundingBox>() && entity->hasComponent<CAnimation>()) {
			const auto& animation = entity->getComponent<CAnimation>().animation;
			if (std::find(pipeTypes.begin(), pipeTypes.end(), animation.getName()) != pipeTypes.end()) {
				pipeXPositions.push_back(entity->getComponent<CTransform>().pos.x);
				auto y = entity->getComponent<CTransform>().pos.y;
				if (animation.getName() == "PipeB")
				{
					y += 0.5f;
				}
				else if (animation.getName() == "PipeS")
				{
					y -= 0.5f;
				}
				pipeYPositions.push_back(y);
			}
		}
	}

	// Spawn Piranha on random pipe positions
	if (!pipeXPositions.empty()) {
		std::random_device rd;
		std::mt19937 gen(rd());
		std::uniform_int_distribution<> dis(0, pipeXPositions.size() - 1);
		int randomIndex = dis(gen);

		auto piranha = m_entityManager.addEntity("Piranha");
		piranha->addComponents<CTransform>(Vec2(pipeXPositions[randomIndex], pipeYPositions[randomIndex] - 32));
		piranha->addComponents<CAnimation>(m_game->getAssets().getAnimation("Piranha"), true);
		piranha->addComponents<CBoundingBox>(m_game->getAssets().getAnimation("Piranha").getSize());
	}
}

void Scene_Play::spawnOwl() {
	std::cout << "Creating Owl\n";
	std::vector<int> groundXPositions;

	// Find all ground positions
	for (const auto& entity : m_entityManager.getEntities()) {
		if (entity->hasComponent<CTransform>() && entity->hasComponent<CBoundingBox>() && entity->hasComponent<CAnimation>()) {
			const auto& animation = entity->getComponent<CAnimation>().animation;
			if (animation.getName() == "Ground") {
				groundXPositions.push_back(entity->getComponent<CTransform>().pos.x);
			}
		}
	}

	// Spawn Owl on random ground positions
	if (!groundXPositions.empty()) {
		std::random_device rd;
		std::mt19937 gen(rd());
		std::uniform_int_distribution<> dis(0, groundXPositions.size() - 1);
		int randomIndex = dis(gen);

		auto owl = m_entityManager.addEntity("Owl");
		owl->addComponents<CTransform>(Vec2(groundXPositions[randomIndex], -64));
		owl->addComponents<CAnimation>(m_game->getAssets().getAnimation("Owl"), true);
		owl->addComponents<CBoundingBox>(m_game->getAssets().getAnimation("Piranha").getSize());
	}
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
	if (input.shoot)
	{
		if (m_player->getComponent<CState>().state != "MarioStandShoot")
		{
			m_player->getComponent<CState>().state = "MarioStandShoot";
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

	for (auto& e : m_entityManager.getEntities("Bullet"))
	{
		e->getComponent<CTransform>().pos += e->getComponent<CTransform>().velocity;
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
	auto& pPos = m_player->getComponent<CTransform>().pos;
	bool hasCollision = false;

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
			if (name == "Castle")
			{
				m_player->destroy();
				onLevelEnd();
			}

			// player reward collosion
			if (name == "Coin" || name == "Star")
			{
				e->destroy();
				m_score += name == "Coin" ? 75 : 125;
			}

			if (name == "MushroomR")
			{
				// need to add some effects up on collision
				e->destroy();
			}

			if (name == "MushroomG")
			{
				// need to add some effects up on collision
				e->destroy();
			}

			// update score
			m_scoreText.setString("Score: " + std::to_string(m_score));

			if (prevOverlap.y > 0.0f)
			{
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
						e->addComponents<CAnimation>(m_game->getAssets().getAnimation("Question2"), true);
						Vec2 pos = e->getComponent<CTransform>().pos;
						pos.y -= 16.5;
						// Seed the random number generator
						std::random_device rd;
						std::mt19937 gen(rd());

						// Choose a random index from 0 to vec.size() - 1
						std::uniform_int_distribution<> dis(0, m_rewards.size() - 1);
						int randomIndex = dis(gen);

						// Get the random element from the vector
						std::string eName = m_rewards[randomIndex];
						auto eCoin = m_entityManager.addEntity(eName);
						eCoin->addComponents<CTransform>(pos);
						eCoin->addComponents<CAnimation>(m_game->getAssets().getAnimation(eName), true);
						eCoin->addComponents<CBoundingBox>(m_game->getAssets().getAnimation(eName).getSize());
					}
					else if (name == "Brick")
					{
						Vec2& pos = e->getComponent<CTransform>().pos;
						e->destroy();
						auto eExplosion = m_entityManager.addEntity("Explosion");
						eExplosion->addComponents<CTransform>(pos);
						eExplosion->addComponents<CAnimation>(m_game->getAssets().getAnimation("Explosion"), false);
					}
				}
				else
				{
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
		pPos.x = 0.0f;
	}

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
				hasCollision = true;
				auto& qPos = e->getComponent<CTransform>().pos;
				auto& bPrevPos = eb->getComponent<CTransform>().prevPos;
				// Check the direction of bullet's movement and handle collision accordingly
				if (eb->getComponent<CTransform>().velocity.x > 0) // Bullet moving right
				{
					if (bPrevPos.x < qPos.x)
					{
						const std::string& name = e->getComponent<CAnimation>().animation.getName();
						if (name == "Brick")
						{
							Vec2& pos = e->getComponent<CTransform>().pos;
							e->destroy();
							eb->destroy();
							auto eExplosion = m_entityManager.addEntity("Explosion");
							eExplosion->addComponents<CTransform>(pos);
							eExplosion->addComponents<CAnimation>(m_game->getAssets().getAnimation("Explosion"), false);
							
						}
						else if (name == "Question" || name == "Question2" || name == "PipeB"
							|| name == "PipeS" || name == "PipeM" || name == "Block" || name == "Solid" || name == "Coin")
						{
							eb->destroy();
						}
					}
				}
				else // Bullet moving left
				{
					if (bPrevPos.x > qPos.x)
					{
						const std::string& name = e->getComponent<CAnimation>().animation.getName();
						if (name == "Brick")
						{
							Vec2& pos = e->getComponent<CTransform>().pos;
							e->destroy();
							eb->destroy();
							auto eExplosion = m_entityManager.addEntity("Explosion");
							eExplosion->addComponents<CTransform>(pos);
							eExplosion->addComponents<CAnimation>(m_game->getAssets().getAnimation("Explosion"), false);
						}
						else if (name == "Question" || name == "Question2" || name == "PipeB"
							|| name == "PipeS" || name == "PipeM" || name == "Block" || name == "Solid")
						{
							eb->destroy();
						}
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

	sf::View originalView = m_game->window().getView();
	sf::View uiView(sf::FloatRect(0, 0, m_game->window().getSize().x, m_game->window().getSize().y));
	m_game->window().setView(uiView);
	m_scoreText.setPosition(10, 10);
	m_game->window().draw(m_scoreText);
	m_game->window().setView(originalView);

	auto& pPos = m_player->getComponent<CTransform>().pos;
	sf::View view = m_game->window().getView();
	float windowCenterX = std::max(view.getSize().x / 2.0f, pPos.x);
	view.setCenter(windowCenterX, m_game->window().getSize().y - view.getSize().y / 2.0f);
	m_game->window().setView(view);

	if (m_drawTexture)
	{
		for (auto& e : m_entityManager.getEntities())
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
	}

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