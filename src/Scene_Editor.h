#pragma once

#include "Scene.h"
#include "Physics.h"
#include <sstream>
#include <map>
#include <memory>

class Scene_Editor : public Scene
{
	struct PlayerConfig
	{
		float X = 0, Y = 0, CX = 0, CY = 0, 
            JUMP = 0, SPEED = 0, MAXSPEED = 0, GRAVITY = 0;
        int HEALTH = 0;
	};

protected:

    std::shared_ptr<Entity> m_player;
    std::string             m_levelPath;
    sf::Text                m_gridText;
    PlayerConfig            m_playerConfig;
    NavMap                  m_navMap;
    bool                    m_drawTextures = true;
    bool                    m_drawCollision = false;
    bool                    m_drawGrid = false;
    bool                    m_drawNav = false;
    Vec2                    m_mousePos;

    void init(const std::string& levelPath);
    void loadLevel(const std::string& filename);
    void loadNavMap(const std::string& filename);
    void saveToFile();

    void onEnd();
    void update();
    void bindActions();
    void openSettings();
    void spawnPlayer();
    void handleClick();
    void handleDestroyClick();
    void handleCopyClick();
    void copyEntity(std::shared_ptr<Entity> entity);
    void insert(std::stringstream& args);
    void updateMousePos(const Vec2& pos);

    void sMovement();
    void sCollision();
    void sAnimation();
    void sCamera();
    void sAutoSave();

public:
    Scene_Editor(GameEngine* gameEngine, const std::string& levelPath);

    void sRender();
    void sDoAction(const Action& action);
};
