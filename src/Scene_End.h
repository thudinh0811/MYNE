#pragma once 

#include "Scene.h"
#include "Common.h"

class Scene_End : public Scene
{
protected:
    std::string                 m_title;
    std::vector<std::string>    m_menuStrings;
    std::string                 m_levelPath;
    sf::Text                    m_menuText;
    size_t                      m_selectedMenuIndex = 0;
    
    void init(const std::string& path);
    void openSettings();
    void bindActions();
    void update();
    void onEnd();
    void sDoAction(const Action& action);
public:
    void sRender();
    Scene_End(GameEngine* game, const std::string& path);
};