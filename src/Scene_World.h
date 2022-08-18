#pragma once

#include "Scene.h"
#include <map>
#include <memory>

class Scene_World : public Scene
{
	struct PlayerConfig
	{
		float X = 0, Y = 0, CX = 0, CY = 0, 
            JUMP = 0, SPEED = 0, MAXSPEED = 0, GRAVITY = 0;
	};

protected:

    std::shared_ptr<Entity> m_player;
    std::string             m_levelPath;
    sf::Text                m_gridText;
    PlayerConfig            m_playerConfig;
    bool                    m_drawTextures = true;
    bool                    m_drawCollision = false;
    bool                    m_drawGrid = false;
    int                     m_doorSize = 768;
    bool                    m_complete[3] = { 0 };

    void init(const std::string& levelPath);
    void loadLevel(const std::string& filename);
    void bindActions();

    void onEnd();
    void openSettings();
    void update();
    void spawnPlayer();

    void sMovement();
    void sCollision();
    void sAnimation();
    void sCamera();

public:
    Scene_World(GameEngine* gameEngine, const std::string& levelPath);

    void sRender();
    void sDoAction(const Action& action);
};
