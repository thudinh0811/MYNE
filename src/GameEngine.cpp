#include "GameEngine.h"
#include "Settings.h"
#include "Assets.h"
#include "Scene_Menu.h"

GameEngine::GameEngine(const std::string & path)
{
    init(path);
}
                          
void GameEngine::init(const std::string & path)
{
    m_assets.loadFromFile(path);
                          
    m_window.create(sf::VideoMode(1280, 768), "MYNE");
    m_window.setFramerateLimit(60);

    Settings::registerAction(sf::Keyboard::W, "UP");
    Settings::registerAction(sf::Keyboard::S, "DOWN");
    Settings::registerAction(sf::Keyboard::D, "RIGHT");
    Settings::registerAction(sf::Keyboard::A, "LEFT");
    Settings::registerAction(sf::Keyboard::E, "NEXT");
    Settings::registerAction(sf::Keyboard::Q, "LAST");
    Settings::registerAction(sf::Keyboard::Space, "USE");

    changeScene("MENU", std::make_shared<Scene_Menu>(this));
}
                          
std::shared_ptr<Scene> GameEngine::currentScene()
{
    return m_sceneMap[m_currentScene];
}
                          
bool GameEngine::isRunning()
{ 
    return m_running & m_window.isOpen();
}

sf::RenderWindow & GameEngine::window()
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
            quit();
        }

        if (event.type == sf::Event::KeyPressed)
        {
        }

                          
        if (event.type == sf::Event::KeyPressed || event.type == sf::Event::KeyReleased)
        {
            if (event.type == sf::Event::KeyPressed)
            {
                currentScene()->getKey(event.key.code);
            }

            // if the current scene does not have an action associated with this key, skip the event
            if (currentScene()->getActionMap().find(event.key.code) == currentScene()->getActionMap().end()) { continue; }
           
            // determine start or end action by whether it was key pres or release
            const std::string actionType = (event.type == sf::Event::KeyPressed) ? "START" : "END";
                        
            // look up the action and send the action to the scene
            currentScene()->doAction(Action(currentScene()->getActionMap().at(event.key.code), actionType));
        }

        auto mpos = sf::Mouse::getPosition(m_window);
        Vec2 pos(mpos.x, mpos.y);
        if (event.type == sf::Event::MouseButtonPressed)
        {
            switch (event.mouseButton.button)
            {
            case sf::Mouse::Left: { currentScene()->doAction(Action("LEFT_CLICK", "START", pos)); break; }
            case sf::Mouse::Middle: { currentScene()->doAction(Action("MIDDLE_CLICK", "START", pos)); break; }
            case sf::Mouse::Right: { currentScene()->doAction(Action("RIGHT_CLICK", "START", pos)); break; }
            }
        }
        if (event.type == sf::Event::MouseButtonReleased)
        {
            switch (event.mouseButton.button)
            {
            case sf::Mouse::Left: { currentScene()->doAction(Action("LEFT_CLICK", "END", pos)); break; }
            case sf::Mouse::Middle: { currentScene()->doAction(Action("MIDDLE_CLICK", "END", pos)); break; }
            case sf::Mouse::Right: { currentScene()->doAction(Action("RIGHT_CLICK", "END", pos)); break; }
            }
        }
        if (event.type == sf::Event::MouseMoved)
        {
            currentScene()->doAction(Action("MOUSE_MOVE", pos));
        }
    }
}

void GameEngine::changeScene(const std::string& sceneName, std::shared_ptr<Scene> scene, bool endCurrentScene)
{
    if (scene)
    {
        m_sceneMap[sceneName] = scene;
    }
    else
    {
        if (m_sceneMap.find(sceneName) == m_sceneMap.end())
        {
            std::cerr << "Warning: Scene does not exist: " << sceneName << std::endl;
            return;
        }
    }

    if (endCurrentScene)
    {
        m_sceneMap.erase(m_sceneMap.find(m_currentScene));
    }

    m_currentScene = sceneName;
}

void GameEngine::update()
{
    if (!isRunning()) { return; }
    
    if (m_sceneMap.empty()) { return; }
    
    sUserInput();
    currentScene()->simulate(m_simulationSpeed);
    currentScene()->sRender();
}

void GameEngine::quit()
{
    m_running = false;
}

Assets& GameEngine::assets()
{
    return m_assets;
}

void GameEngine::playSound(const std::string& soundName)
{
    m_assets.getSound(soundName).play();
}

void GameEngine::playMusic(const std::string& soundName)
{
    m_assets.getMusic(soundName).play();
}

void GameEngine::setMusicVolume(int v)
{
    m_assets.setMusicVolume(v);
}

void GameEngine::setSFXVolume(int v)
{
    m_assets.setSFXVolume(v);
}

// Copyright (C) David Churchill - All Rights Reserved
// COMP4300 - 2021FALL - Assignment 4
// Written by David Churchill (dave.churchill@gmail.com)
// Unauthorized copying of these files are strictly prohibited
// Distributed only for course work at Memorial University
// If you see this file online please contact email above
