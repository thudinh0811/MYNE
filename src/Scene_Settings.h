#pragma once

#include "Common.h"
#include "Scene.h"
#include "GameEngine.h"
#include <map>
#include <memory>
#include <deque>

#include "EntityManager.h"

class Scene_Settings : public Scene
{

protected:

    std::string                 m_returnScene;
    std::string                 m_title = "Settings";
    std::vector<std::string>    m_menuStrings;
    std::vector<std::string>    m_levelPaths;
    std::vector<std::string>    m_actionStrings;
    sf::Text                    m_menuText;
    sf::RectangleShape          m_square;
    size_t                      m_selectedMenuIndex = 0;
    size_t                      m_actionSwapIndex = 0;
    bool                        m_captureNextInput = false;
    
    void init();
    void bindActions();
    void getKey(int code);
    void update();
    void openSettings();
    void onEnd();
    void sDoAction(const Action& action);

public:

    Scene_Settings(GameEngine * gameEngine, const std::string& lastScene);
    void sRender();
};
