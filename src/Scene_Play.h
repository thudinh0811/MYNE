#pragma once

#include "Scene.h"
#include "Inventory.h"
#include "Parallax.h"
#include <memory>

class Scene_Play : public Scene
{
    struct PlayerConfig
	{
		float X = 0, Y = 0, CX = 0, CY = 0, 
            JUMP = 0, SPEED = 0, MAXSPEED = 0, GRAVITY = 0;
        int HEALTH = 0;
	};

protected:
    int                     m_levelID;
    std::shared_ptr<Entity> m_player;
    std::string             m_levelPath;
    std::string             m_levelMusic;
    sf::Text                m_gridText;
    Animation               m_healthBar;
    Animation               m_invPeek[3];
    sf::RectangleShape      m_invBox;
    sf::Shader              m_shader;
    sf::View                m_minimap;
    PlayerConfig            m_playerConfig;
    NavMap                  m_navMap;
    Inventory               m_playerInventory;
    Parallax                m_parallax;
    sf::Sprite              m_screen;
    sf::Texture             m_screenTexture;
    bool                    m_flipview = false;
    bool                    m_drawTextures = true;
    bool                    m_drawCollision = false;
    bool                    m_drawGrid = false;
    bool                    m_drawNav = false;
    bool                    m_shift = false;
    bool                    m_canAttack = true;
    bool                    m_bossClear = false;

    void init(const std::string& levelPath);
    void loadLevel(const std::string& filename);
    void loadNavMap(const std::string& filename);

    void onComplete();
    void onEnd();
    void bindActions();
    void openSettings();
    void onEdit();
    void update();
    void spawnPlayer();
    void loadShader(const std::string& shaderPath);
    void moveToTarget(Vec2 target, std::shared_ptr<Entity> entity, float speed);

    void sAI();
    void sInput();
    void sState();
    void sInventory();
    void sMovement();
    void sCollision();
    void sAnimation();
    void sCamera();

public:
    Scene_Play(GameEngine* gameEngine, const std::string& levelPath);

    void sRender();
    void sDoAction(const Action& action);
    bool canSee(std::shared_ptr<Entity> target, std::shared_ptr<Entity> looker);
    bool canSee(std::shared_ptr<Entity> target, Vec2 lookPos);
    bool canSee(Vec2 target, Vec2 looker);

};