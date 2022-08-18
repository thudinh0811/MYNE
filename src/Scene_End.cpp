#include "Scene_End.h"
#include "GameEngine.h"
#include "Scene_Settings.h"
#include "Scene_Menu.h"
#include "Settings.h"

Scene_End::Scene_End(GameEngine* game, const std::string& path)
	: Scene(game)
	, m_levelPath(path)
{
	init(path);
}

void Scene_End::init(const std::string& path)
{
    bindActions();
    auto mona = m_entityManager.addEntity("pic");
    mona->addComponent<CTransform>(getPosition(6, 6));
    mona->addComponent<CAnimation>(m_game->assets().getAnimation("MonaDown"), true);
    auto princess = m_entityManager.addEntity("pic");
    princess->addComponent<CTransform>(getPosition(10, 6));
    princess->addComponent<CAnimation>(m_game->assets().getAnimation("PrincessEnd"), true);

    m_menuText.setCharacterSize(48);
    m_menuText.setFillColor(sf::Color::White);
    m_menuText.setFont(m_game->assets().getFont("DPComic"));

    std::ifstream fin(path);

    while (fin.good())
    {
        std::string line;
        std::getline(fin, line);
        m_menuStrings.push_back(line);
    }

    m_game->playMusic("BossTheme");
}

void Scene_End::openSettings()
{
	m_game->changeScene("SETTINGS", std::make_shared<Scene_Settings>(m_game, "END"));
}

void Scene_End::bindActions()
{
    registerAction(sf::Keyboard::Escape, "QUIT");

    for (const auto& [key, value] : Settings::keyMap)
    {
        registerAction(key, value);
    }
}

void Scene_End::update()
{
    m_entityManager.update();
    auto view = m_game->window().getDefaultView();
    view.setCenter(view.getCenter() + sf::Vector2f(0, m_currentFrame));
    m_game->window().setView(view);

    if (view.getCenter().y > m_menuStrings.size() * 100 + 700)
    {
        onEnd();
    }
    m_currentFrame++;
}

void Scene_End::onEnd()
{
    m_game->assets().getMusic("BossTheme").stop();
    m_game->changeScene("MENU", nullptr, true);
    m_game->playMusic("MainTheme");
}

void Scene_End::sDoAction(const Action& action)
{
    if (action.type() == "START")
    {
        if (action.name() == "USE")
        {
            onEnd();
        }
        else if (action.name() == "QUIT")
        {
            onEnd();
        }
    }
}

void Scene_End::sRender()
{
    m_game->window().clear(sf::Color(0, 0, 0));
    for (auto entity : m_entityManager.getEntities("pic"))
    {
        auto& trans = entity->getComponent<CTransform>();
        auto& anim = entity->getComponent<CAnimation>();
        anim.animation.getSprite().setPosition(trans.pos.x, trans.pos.y);
        m_game->window().draw(anim.animation.getSprite());
    }

    for (size_t i = 0; i < m_menuStrings.size(); i++)
    {
        m_menuText.setString(m_menuStrings[i]);
        m_menuText.setPosition(200, 400 + i * 100);
        m_game->window().draw(m_menuText);
    }
    m_game->window().display();
}
