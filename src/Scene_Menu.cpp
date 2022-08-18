#include "Scene_Settings.h"
#include "Settings.h"
#include "Scene_Menu.h"
#include "Scene_World.h"
#include "Common.h"
#include "Assets.h"
#include "GameEngine.h"
#include "Components.h"
#include "Action.h"
#include "Scene_End.h"

Scene_Menu::Scene_Menu(GameEngine * gameEngine)
    : Scene(gameEngine)
{
    init();
}

void Scene_Menu::init()
{                          
    bindActions();
    m_title = "MYNE";
    m_menuStrings.push_back("Play");
    m_menuStrings.push_back("Settings");
    m_menuStrings.push_back("Credits");

    m_levelPaths.push_back("level1.txt");
    m_levelPaths.push_back("level2.txt");
    m_levelPaths.push_back("level.txt");

    m_menuText.setFont(m_game->assets().getFont("DPComic"));
    m_menuText.setCharacterSize(64);

    auto ent = m_entityManager.addEntity("title");
    ent->addComponent<CTransform>(Vec2(m_game->window().getSize().x / 2, m_game->window().getSize().y / 2));
    ent->addComponent<CAnimation>(m_game->assets().getAnimation("TitleBackground"), true);
                          
    auto e = m_entityManager.addEntity("title");
    e->addComponent<CTransform>(Vec2(m_game->window().getSize().x / 2 - 25, m_game->window().getSize().y / 2));
    e->addComponent<CAnimation>(m_game->assets().getAnimation("Title"), true);

    m_game->playMusic("MainTheme");
}

void Scene_Menu::bindActions()
{
    registerAction(sf::Keyboard::Escape, "QUIT");

    for (const auto& [key, value] : Settings::keyMap)
    {
        registerAction(key, value);
    }
}

void Scene_Menu::update()
{
    m_entityManager.update();
}

void Scene_Menu::sDoAction(const Action& action)
{
    if (action.type() == "START")
    {
        if (action.name() == "LEFT")
        {
            if (m_selectedMenuIndex > 0) { m_selectedMenuIndex--; }
            else { m_selectedMenuIndex = m_menuStrings.size() - 1; }
        }
        else if (action.name() == "RIGHT")
        {
            m_selectedMenuIndex = (m_selectedMenuIndex + 1) % m_menuStrings.size();
        }
        else if (action.name() == "USE")
        {
            if (m_selectedMenuIndex == 0)
            {
                m_game->changeScene("WORLD", std::make_shared<Scene_World>(m_game, "world_view.txt"));
            }
            else if (m_selectedMenuIndex == 1)
            {
                openSettings();
            }
            else if (m_selectedMenuIndex == 2)
            {
                m_game->assets().getMusic("MainTheme").stop();
                m_game->changeScene("END", std::make_shared<Scene_End>(m_game, "credits.txt"));
            }
        }
        else if (action.name() == "QUIT")
        {
            onEnd();
        }
    }
}

void Scene_Menu::openSettings()
{
	m_game->changeScene("SETTINGS", std::make_shared<Scene_Settings>(m_game, "MENU"));
}

void Scene_Menu::sRender()
{
    // clear the window to a blue
    m_game->window().setView(m_game->window().getDefaultView());
    m_game->window().clear(sf::Color(0, 0, 0));

    for (auto entity : m_entityManager.getEntities())
    {
        auto& trans = entity->getComponent<CTransform>();
        auto& animation = entity->getComponent<CAnimation>().animation;
        animation.getSprite().setPosition(trans.pos.x, trans.pos.y);
        animation.getSprite().setScale(trans.scale.x, trans.scale.y);
        m_game->window().draw(animation.getSprite());
    }


    // draw the game title in the top-left of the screen
    m_menuText.setCharacterSize(48);
    
    // draw all of the menu options
    for (size_t i = 0; i < m_menuStrings.size(); i++)
    {
        m_menuText.setString(m_menuStrings[i]);
        m_menuText.setFillColor(i == m_selectedMenuIndex ? sf::Color::White : sf::Color(100, 100, 100));
        m_menuText.setPosition(sf::Vector2f(300 + i*250 + i*i*5, 70));
        m_game->window().draw(m_menuText);
    }
                          
    // draw the controls in the bottom-left
    m_menuText.setCharacterSize(20);
    m_menuText.setFillColor(sf::Color(100, 100, 100));
    m_menuText.setString("left: a     right: d    select: space      back: esc");
    m_menuText.setPosition(sf::Vector2f(10, 690));
    m_game->window().draw(m_menuText);

    m_game->window().display();
}

void Scene_Menu::onEnd()
{
    m_hasEnded = true;
    m_game->quit();
}

// Copyright (C) David Churchill - All Rights Reserved
// COMP4300 - 2021FALL - Assignment 4
// Written by David Churchill (dave.churchill@gmail.com)
// Unauthorized copying of these files are strictly prohibited
// Distributed only for course work at Memorial University
// If you see this file online please contact email above
