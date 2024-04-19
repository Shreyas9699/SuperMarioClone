#include "Scene_Menu.h"
#include "Scene_Play.h"
#include "GameEngine.h"

Scene_Menu::Scene_Menu(GameEngine* game)
	: Scene(game)
{
	init("");
}

void Scene_Menu::init(const std::string& menuConfig)
{
	registerAction(sf::Keyboard::W, "UP");
	registerAction(sf::Keyboard::S, "DOWN");
	registerAction(sf::Keyboard::Enter, "PLAY");
	registerAction(sf::Keyboard::Q, "QUIT");
	registerAction(sf::Keyboard::Escape, "QUIT");

	m_title = "Mega Mario";
	m_menuStrings.push_back("Level  1");
	m_menuStrings.push_back("Level  2");
	m_menuStrings.push_back("Level  3");

	m_levelPaths.push_back("assets/levels/level1.txt");
	m_levelPaths.push_back("assets/levels/level2.txt");
	m_levelPaths.push_back("assets/levels/level3.txt");

	m_menuText.setFont(m_game->getAssets().getFont("Megaman"));
	m_menuText.setCharacterSize(40); 
	m_menuInstText.setFont(m_game->getAssets().getFont("Pixel"));
	m_menuInstText.setFillColor(sf::Color::Black);
	m_menuInstText.setString("Up : W    DOWN : S    PLAY : ENTER    QUIT : ESC");
	m_menuInstText.setCharacterSize(m_menuText.getCharacterSize() / 2);
	m_menuInstText.setPosition(20.0f, m_game->window().getSize().y - 30.0f); // Position at bottom left
	std::cout << "MENU Scene init Completed!" << std::endl;
}

void Scene_Menu::update()
{
	m_entityManager.update();
	m_game->window().setView(m_game->window().getDefaultView());
}

void Scene_Menu::sRender()
{
	float verticalSpacing = 60.0f; // Vertical spacing between each text
	float initialYPosition = 80.0f; // Initial vertical position
	auto& window = m_game->window();
	window.clear(sf::Color(77, 77, 255));

	m_menuText.setFillColor(sf::Color::Black);
	m_menuText.setString(m_title);
	m_menuText.setPosition(20.0f,  40.0f);
	window.draw(m_menuText);
	
	for (size_t i = 1; i <= m_menuStrings.size() ; i++)
	{
		if (i - 1 == m_selectedMenuIdx) // Check if this text is selected
		{
			m_menuText.setFillColor(sf::Color::White); // Set color to white if selected
		}
		else
		{
			m_menuText.setFillColor(sf::Color::Black); // Set color to black if not selected
		}
		m_menuText.setString(m_menuStrings[i - 1]);
		m_menuText.setPosition(20.0f, initialYPosition + i * (verticalSpacing + 20.0f));
		window.draw(m_menuText);
	}
	window.draw(m_menuInstText);
}

void Scene_Menu::onEnd()
{
	m_game->quit();
}

void Scene_Menu::sDoAction(const Action& action)
{
	if (action.type() == "START")
	{
		if (action.name() == "UP")
		{
			if (m_selectedMenuIdx > 0)
			{
				m_selectedMenuIdx -= 1;
			}
			else
			{
				m_selectedMenuIdx = m_menuStrings.size() - 1;
			}
		}
		else if (action.name() == "DOWN")
		{
			m_selectedMenuIdx = (m_selectedMenuIdx + 1) % m_menuStrings.size();
		}
		else if (action.name() == "PLAY")
		{
			m_game->changeScene("PLAY", std::make_shared<Scene_Play>(m_game, m_levelPaths[m_selectedMenuIdx]));
		}
		else if (action.name() == "QUIT")
		{
			onEnd();
		}
	}
}