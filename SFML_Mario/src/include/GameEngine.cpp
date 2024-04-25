#include <iostream>
#include "GameEngine.h"
#include "Scene_Menu.h"

/*
    Stores top level game data : Assets, sf::Window, Scenes
    Performs top level functionality; changing Scenes, handling inputs
    Run() contains game main loop
*/

int wWidht  = 1600;
int wHeight = 900;
//1280x720, 1920x1080, or 2560x1440 

GameEngine::GameEngine(const std::string& path)
{
    init(path);
}


void GameEngine::init(const std::string& path)
{
    m_assets.loadFromFile(path);
    m_window.create(sf::VideoMode(wWidht, wHeight), "Definitely Not Mario");
    m_window.setFramerateLimit(60);
    m_window.setVerticalSyncEnabled(true);
    changeScene("MENU", std::make_shared<Scene_Menu>(this));
    std::cout << "Completed initializing the Game window and create Menu" << std::endl;
}

void GameEngine::update()
{
    if (!isRunning() || m_sceneMap.empty())
    {
        return;
    }
    sUserInput();
    getCurrentScene()->simulate(m_simulationSpeed);
    getCurrentScene()->sRender();
    m_window.display();
}

bool GameEngine::isRunning() const
{
    return m_running && m_window.isOpen();
}

sf::RenderWindow& GameEngine::window()
{
    return m_window;
}

void GameEngine::run()
{
    while (isRunning())
    {
        update();
    }
}

void GameEngine::sUserInput()
{
    sf::Event event;
    while (m_window.pollEvent(event))
    {
        if (event.type == sf::Event::Closed)
        {
            std::cout << "Exiting the game!\n";
            quit();
        }
        if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::X)
        {
            std::cout << "trying to save screeshot to " << "test.png" << std::endl;
            sf::Texture t;
            t.create(m_window.getSize().x, m_window.getSize().y);
            t.update(m_window);
            if (t.copyToImage().saveToFile("test.png"))
            {
                std::cout << "screenshot saved to " << "test.png" << std::endl;
            }
        }
        if (event.type == sf::Event::KeyPressed || event.type == sf::Event::KeyReleased)
        {
            if (getCurrentScene()->getActionMap().find(event.key.code) == getCurrentScene()->getActionMap().end())
            {
                continue;
            }
            const std::string actionType = (event.type == sf::Event::KeyPressed) ? "START" : "END";
            getCurrentScene()->sDoAction(Action(getCurrentScene()->getActionMap().at(event.key.code), actionType));
        }
    }
}

void GameEngine::changeScene(std::string sceneName, std::shared_ptr<Scene> scene, bool endCurrentScene)
{
    if (scene)
    {
        m_sceneMap[sceneName] = scene;
    }
    else
    {
        if (m_sceneMap.find(sceneName) == m_sceneMap.end())
        {
            std::cerr << "Warning: Scene does not exisits : " << sceneName << std::endl;
            return;
        }
    }

    if (endCurrentScene)
    {
        m_sceneMap.erase(m_sceneMap.find(m_currentScene));
    }
    // update current scene
    m_currentScene = sceneName;

}

void GameEngine::quit()
{
    m_running = false;
}

const Assets& GameEngine::getAssets()
{
    return m_assets;
}

std::shared_ptr<Scene> GameEngine::getCurrentScene()
{
    return m_sceneMap[m_currentScene];
}