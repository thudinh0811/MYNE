#include "Scene_Settings.h"
#include "Settings.h"

Scene_Settings::Scene_Settings(GameEngine* gameEngine, const std::string& lastScene)
	:	Scene(gameEngine)
	,	m_returnScene(lastScene)
{
	init();
}

void Scene_Settings::init()
{
    bindActions();
    m_menuStrings.push_back("Difficulty");
    m_menuStrings.push_back("Music Volume");
    m_menuStrings.push_back("SFX Volume");
    m_menuStrings.push_back("Set Keys");

    m_actionStrings.push_back("UP");
    m_actionStrings.push_back("DOWN");
    m_actionStrings.push_back("LEFT");
    m_actionStrings.push_back("RIGHT");
    m_actionStrings.push_back("USE");
    m_actionStrings.push_back("NEXT");
    m_actionStrings.push_back("LAST");

    m_square = sf::RectangleShape(sf::Vector2f(32, 32));
    
    m_menuStrings.push_back("Back");

    m_menuText.setFont(m_game->assets().getFont("DPComic"));
    m_menuText.setCharacterSize(64);
}

void Scene_Settings::bindActions()
{
    registerAction(sf::Keyboard::Escape, "QUIT");

    for (const auto& [key, value] : Settings::keyMap)
    {
        registerAction(key, value);
    }

}

void Scene_Settings::update()
{
}
void Scene_Settings::openSettings()
{
}

void Scene_Settings::onEnd()
{
    m_game->changeScene(m_returnScene, nullptr, true);
}

void Scene_Settings::getKey(int code)
{
    if (m_captureNextInput)
    {
        Settings::registerAction(code, m_actionStrings[m_actionSwapIndex]);
        m_captureNextInput = false;
    }
}

void Scene_Settings::sDoAction(const Action& action)
{
    if (action.type() == "START")
    {
        if (m_captureNextInput)
        {}
        else if (action.name() == "UP")
        {
            if (m_selectedMenuIndex > 0) { m_selectedMenuIndex--; }
            else { m_selectedMenuIndex = m_menuStrings.size() - 1; }
        }
        else if (action.name() == "DOWN")
        {
            m_selectedMenuIndex = (m_selectedMenuIndex + 1) % m_menuStrings.size();
        }
        else if (action.name() == "LEFT")
        {
            if (m_selectedMenuIndex == 0)
            {
                Settings::difficulty = (Settings::Difficulty)(fmax(0, (int)Settings::difficulty - 1));
            }
            else if (m_selectedMenuIndex == 1)
            {
                if (Settings::musicVolume > 0) { Settings::musicVolume--; }
            }
            else if (m_selectedMenuIndex == 2)
            {
                if (Settings::sfxVolume > 0) { Settings::sfxVolume--; }
            }
            else if (m_selectedMenuIndex == 3)
            {
                if (m_actionSwapIndex > 0) { m_actionSwapIndex--; }
            }
        }
        else if (action.name() == "RIGHT")
        {
            if (m_selectedMenuIndex == 0)
            {
                Settings::difficulty = (Settings::Difficulty)(fmin(2, (int)Settings::difficulty + 1));
            }
            else if (m_selectedMenuIndex == 1)
            {
                if (Settings::musicVolume < 9) { Settings::musicVolume++; }
            }
            else if (m_selectedMenuIndex == 2)
            {
				if (Settings::sfxVolume < 9) { Settings::sfxVolume++; }
            }
            else if (m_selectedMenuIndex == 3)
            {
                if (m_actionSwapIndex < 6) { m_actionSwapIndex++; }
            }
        }

        else if (action.name() == "USE")
        {
            if (m_selectedMenuIndex == 4)
            {
                onEnd();
            }
        }

        else if (action.name() == "QUIT")
        {
            onEnd();
        }

        m_game->setMusicVolume(Settings::musicVolume);
        m_game->setSFXVolume(Settings::sfxVolume);
    }
    else if (action.type() == "END")
    {
        if (action.name() == "USE")
        {
            if (m_selectedMenuIndex == 3)
            {
                auto it = std::find_if(Settings::keyMap.begin(), Settings::keyMap.end(), [&](auto& it) { return it.second == m_actionStrings[m_actionSwapIndex]; });
                Settings::keyMap.erase(it);
                m_captureNextInput = true;
            }
        }
    }
}

void Scene_Settings::sRender()
{
    m_game->window().setView(m_game->window().getDefaultView());
    m_game->window().clear(sf::Color(0, 0, 0));

    float x = 50.f;

    m_menuText.setFillColor(sf::Color::White);
    m_menuText.setCharacterSize(48);
    m_menuText.setPosition(x, 100.f);
    m_menuText.setString(m_title);
    m_game->window().draw(m_menuText);

    m_menuText.setString(m_menuStrings[0]);
    m_menuText.setFillColor(m_selectedMenuIndex == 0 ? sf::Color::White : sf::Color(140,140,140));
    m_menuText.setCharacterSize(36);
    m_menuText.setPosition(x, 200);
    m_game->window().draw(m_menuText);
    for (int i = 0; i < 3; i++)
    {
        m_square.setPosition(x + i * 64, 260);
        m_square.setFillColor(i == (int)Settings::difficulty ? sf::Color::White : sf::Color(100, 100, 100));
        m_game->window().draw(m_square);
    }
    
    // draw the sound menu options
    for (size_t i = 0; i < 2; i++)
    {
		m_menuText.setFillColor(m_selectedMenuIndex == i + 1 ? sf::Color::White : sf::Color(140,140,140));
        m_menuText.setString(m_menuStrings[i + 1]);
        m_menuText.setPosition(sf::Vector2f(x, 310 + i * 100));
        m_game->window().draw(m_menuText);
        int vol = i == 0 ? Settings::musicVolume : Settings::sfxVolume;

        for (int j = 0; j < 10; j++)
        {
            m_square.setPosition(x + j * 64, m_menuText.getPosition().y + 60);
			m_square.setFillColor(j == vol ? sf::Color::White : sf::Color(100, 100, 100));
            m_game->window().draw(m_square);
        }
    }

	m_menuText.setString(m_menuStrings[3]);
    m_menuText.setFillColor(m_selectedMenuIndex == 3 ? sf::Color::White : sf::Color(140,140,140));
    m_menuText.setPosition(x, 550);
    m_game->window().draw(m_menuText);

    int ks[7] = { 0 };
    for (auto& [key, value] : Settings::keyMap)
    {
        auto it = std::find(m_actionStrings.begin(), m_actionStrings.end(), value);
        if (it != m_actionStrings.end())
        {
            ks[it - m_actionStrings.begin()] = key;
        }
        else
        {
            std::cout << "NOT FOUND!!\n";
        }
    }


    for (size_t i = 0; i < 7; i++)
    {
        m_menuText.setString(m_actionStrings[i] + ':' + (char)(ks[i] == 57 ? '_' : ks[i] + 'A'));
		m_menuText.setFillColor(m_actionSwapIndex == i ? sf::Color::White : sf::Color(140,140,140));
        m_menuText.setPosition(x + 150 * i, 600);
        m_game->window().draw(m_menuText);
    }
                          
    m_menuText.setFillColor(m_selectedMenuIndex == 4 ? sf::Color::White : sf::Color(140,140,140));
	m_menuText.setString(m_menuStrings[4]);
	m_menuText.setPosition(sf::Vector2f(x, 650));
    m_game->window().draw(m_menuText);
    
    m_game->window().display();
}
