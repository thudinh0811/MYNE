#include "Scene_World.h"
#include "Scene_Play.h"
#include "Scene_Settings.h"
#include "Settings.h"
#include "Physics.h"
#include "Assets.h"
#include "GameEngine.h"
#include "Components.h"

Scene_World::Scene_World(GameEngine* game, const std::string& levelPath)
	: Scene(game)
	, m_levelPath(levelPath)
{
	init(m_levelPath);
}

void Scene_World::init(const std::string& levelPath)
{

    std::fstream file("pg.txt");
    file >> m_complete[0] >> m_complete[1] >> m_complete[2];

	loadLevel(levelPath);

    bindActions();
    m_gridText.setCharacterSize(12);
    m_gridText.setFont(m_game->assets().getFont("DPComic"));
    spawnPlayer();
}

void Scene_World::loadLevel(const std::string& filename)
{
    m_entityManager = EntityManager();
    std::fstream fin(filename);
    bool unlock = true;
    int index = 0;

    std::string type;
    while (fin.good())
    {
        fin >> type;
        if (type == "Player")
        {
            fin >> m_playerConfig.X >> m_playerConfig.Y >> m_playerConfig.CX >> m_playerConfig.CY
                >> m_playerConfig.JUMP >> m_playerConfig.SPEED >> m_playerConfig.MAXSPEED >> m_playerConfig.GRAVITY;
        }

        if (type == "Tile")
        {
            std::string anim;
            int gx, gy, bm, bv;
            fin >> anim >> gx >> gy >> bm >> bv;
            auto tile = m_entityManager.addEntity("tile");
            tile->addComponent<CAnimation>(m_game->assets().getAnimation(anim), true);
            tile->addComponent<CTransform>(getPosition(gx, gy));
            tile->addComponent<CBoundingBox>(m_game->assets().getAnimation(anim).getSize(), bm, bv);
        }

        if (type == "Door")
        {
            std::string anim, lvl;
            float gx, gy, cx, cy, bm, bv;
            fin >> anim >> gx >> gy >> cx >> cy >> bm >> bv >> lvl;
            auto tile = m_entityManager.addEntity("door");
            tile->addComponent<CTransform>(getPosition(gx, gy));
            if (unlock)
            {
                unlock = m_complete[index++];
                tile->addComponent<CBoundingBox>(Vec2(cx, cy), false, false);
            }
            else
            {
                auto lock = m_entityManager.addEntity("lock");
                lock->addComponent<CTransform>(getPosition(gx, gy - 2));
                lock->addComponent<CAnimation>(m_game->assets().getAnimation("Lock"), true);
            }

            if (unlock)
            {
                anim += "Open";
            }
            else
            {
                anim += "Closed";
            }

            tile->addComponent<CAnimation>(m_game->assets().getAnimation(anim), true);
            tile->addComponent<CDoor>(lvl);
        }
    }
}

void Scene_World::bindActions()
{
    registerAction(sf::Keyboard::Escape, "QUIT");
    registerAction(sf::Keyboard::P, "PAUSE");
    registerAction(sf::Keyboard::T, "TOGGLE_TEXTURE");      // Toggle drawing (T)extures
    registerAction(sf::Keyboard::G, "TOGGLE_GRID");    // Toggle drawing grid lines
    registerAction(sf::Keyboard::C, "TOGGLE_COLLISION");    // Toggle drawing (C)ollision Boxes
    
    for (const auto& [key, value] : Settings::keyMap)
    {
        registerAction(key, value);
    }
}

void Scene_World::spawnPlayer()
{
    m_player = m_entityManager.addEntity("player");
    m_player->addComponent<CTransform>(getPosition(m_playerConfig.X, m_playerConfig.Y));
    m_player->addComponent<CAnimation>(m_game->assets().getAnimation("Stand"), true);
    m_player->addComponent<CBoundingBox>(Vec2(m_playerConfig.CX, m_playerConfig.CY), true, false);
    m_player->addComponent<CInput>();
    m_player->addComponent<CGravity>(m_playerConfig.GRAVITY);
}

void Scene_World::onEnd()
{
    m_game->changeScene("MENU", nullptr, true);
}

void Scene_World::openSettings()
{
    m_game->changeScene("SETTINGS", std::make_shared<Scene_Settings>(m_game, "WORLD"));
}

void Scene_World::update()
{
    if (!m_paused)
    {
        m_entityManager.update();
        sMovement();
        sCollision();
        sCamera();
        sAnimation();
        m_currentFrame++;
    }
}

void Scene_World::sMovement()
{
    for (auto entity : m_entityManager.getEntities())
    {
        auto& trans = entity->getComponent<CTransform>();
        trans.prevPos = trans.pos;
        if (entity->hasComponent<CGravity>())
        {
            trans.velocity.y += entity->getComponent<CGravity>().gravity;
        }
        if (entity->hasComponent<CInput>())
        {
            auto& input = entity->getComponent<CInput>();
            if (!input.up && trans.velocity.y < 0)
            {
                trans.velocity.y = 0;
            }
            if (!(input.right ^ input.left))
            {
                trans.velocity.x = 0;
            }
            else if (input.right)
            {
                trans.velocity.x = m_playerConfig.SPEED;
                trans.scale.x = 1;
            }
            else if (input.left)
            {
                trans.velocity.x = -m_playerConfig.SPEED;
                trans.scale.x = -1;
            }

            float mag = trans.velocity.dist(Vec2());
            if (mag > m_playerConfig.MAXSPEED)
            {
                trans.velocity *= m_playerConfig.MAXSPEED / mag;
            }
        }

        trans.pos += trans.velocity;
    }
}

void Scene_World::sCollision()
{
    auto& playerTrans = m_player->getComponent<CTransform>();
    auto& playerInput = m_player->getComponent<CInput>();
    m_player->getComponent<CGravity>().canJump = false;

    for (auto tile : m_entityManager.getEntities("tile"))
    {
        Vec2 overlap = Physics::GetOverlap(m_player, tile);
        if (overlap.x > 0 && overlap.y > 0)
        {
            Vec2 prevOverlap = Physics::GetPreviousOverlap(m_player, tile);
            if (prevOverlap.x > 0 || (prevOverlap.x > 0 && prevOverlap.y > 0))
            {
                bool above = playerTrans.pos.y < tile->getComponent<CTransform>().pos.y;
                playerTrans.pos.y -= above ? overlap.y : -overlap.y;
                m_player->getComponent<CGravity>().canJump = above;
                playerTrans.velocity.y = 0;
            }
            else if (prevOverlap.y > 0)
            {
                playerTrans.pos.x -= playerTrans.pos.x < tile->getComponent<CTransform>().pos.x ? overlap.x : -overlap.x;
            }

        }
    }

    if (playerInput.down)
    {
        for (auto door : m_entityManager.getEntities("door"))
        {
            Vec2 overlap = Physics::GetOverlap(m_player, door);
            if (overlap.x > 0 && overlap.y > 0)
            {
                m_game->assets().getMusic("MainTheme").stop();
                m_game->changeScene("PLAY", std::make_shared<Scene_Play>(m_game, door->getComponent<CDoor>().leveltxt), true);
                m_player->addComponent<CInput>();
            }
        }
        playerInput.down = false;
    }

    if (playerTrans.pos.x < m_player->getComponent<CBoundingBox>().halfSize.x)
    {
        playerTrans.pos.x = m_player->getComponent<CBoundingBox>().halfSize.x;
    }
    else if (playerTrans.pos.x > (m_doorSize * 3 - m_player->getComponent<CBoundingBox>().halfSize.x))
    {
        playerTrans.pos.x = m_doorSize * 3 - m_player->getComponent<CBoundingBox>().halfSize.x;
    }
}

void Scene_World::sCamera()
{
    sf::View view = m_game->window().getView();
    view.setCenter(fmax(m_player->getComponent<CTransform>().pos.x, m_game->window().getSize().x / 2), view.getCenter().y);
    m_game->window().setView(view);
}

void Scene_World::sAnimation()
{
    auto& playerState = m_player->getComponent<CState>();
    auto& playerAnim = m_player->getComponent<CAnimation>();
    if (m_player->getComponent<CGravity>().canJump)
    {
        playerState.state = m_player->getComponent<CTransform>().velocity.x ? "Run" : "Stand";
    }
    else
    {
        playerState.state = "Air";
    }

    if (playerState.state != playerAnim.animation.getName())
    {
        m_player->addComponent<CAnimation>(m_game->assets().getAnimation(playerState.state), true);
    }

    for (auto entity : m_entityManager.getEntities())
    {
        auto& anim = entity->getComponent<CAnimation>();
        anim.animation.update();
        if (!anim.repeat && anim.animation.hasEnded())
        {
            entity->destroy();
        }
    }

}

void Scene_World::sRender()
{
    m_game->window().clear(sf::Color(0, 0, 0));
                          
    // draw all Entity textures / animations
    if (m_drawTextures)
    {
        // draw entity animations
        for (auto e : m_entityManager.getEntities())
        {
            auto& transform = e->getComponent<CTransform>();
            sf::Color c = sf::Color::White;
            if (e->hasComponent<CInvincibility>())
            {
                c = sf::Color(255, 255, 255, 128);
            }
                          
            if (e->hasComponent<CAnimation>())
            {
                auto& animation = e->getComponent<CAnimation>().animation;
                animation.getSprite().setRotation(transform.angle);
                animation.getSprite().setPosition(transform.pos.x, transform.pos.y);
                animation.getSprite().setScale(transform.scale.x, transform.scale.y);
                animation.getSprite().setColor(c);
                m_game->window().draw(animation.getSprite());
            }
        }
    }

    // draw all Entity collision bounding boxes with a rectangleshape
    if (m_drawCollision)
    {
        sf::CircleShape dot(4);
        dot.setFillColor(sf::Color::Black);
        for (auto e : m_entityManager.getEntities())
        {
            if (e->hasComponent<CBoundingBox>())
            {
                auto& box = e->getComponent<CBoundingBox>();
                auto& transform = e->getComponent<CTransform>();
                sf::RectangleShape rect;
                rect.setSize(sf::Vector2f(box.size.x - 1, box.size.y - 1));
                rect.setOrigin(sf::Vector2f(box.halfSize.x, box.halfSize.y));
                rect.setPosition(transform.pos.x, transform.pos.y);
                rect.setFillColor(sf::Color(0, 0, 0, 0));

                if (box.blockMove && box.blockVision) { rect.setOutlineColor(sf::Color::Black); }
                if (box.blockMove && !box.blockVision) { rect.setOutlineColor(sf::Color::Blue); }
                if (!box.blockMove && box.blockVision) { rect.setOutlineColor(sf::Color::Red); }
                if (!box.blockMove && !box.blockVision) { rect.setOutlineColor(sf::Color::White); }
                rect.setOutlineThickness(1);
                m_game->window().draw(rect);
            }

            if (e->hasComponent<CPatrol>())
            {
                auto& patrol = e->getComponent<CPatrol>().positions;
                for (size_t p = 0; p < patrol.size(); p++)
                {
                    dot.setPosition(patrol[p].x, patrol[p].y);
                    m_game->window().draw(dot);
                }
            }
                          
            if (e->hasComponent<CFollowPlayer>())
            {
                sf::VertexArray lines(sf::LinesStrip, 2);
                lines[0].position.x = e->getComponent<CTransform>().pos.x;
                lines[0].position.y = e->getComponent<CTransform>().pos.y;
                lines[0].color = sf::Color::Black;
                lines[1].position.x = m_player->getComponent<CTransform>().pos.x;
                lines[1].position.y = m_player->getComponent<CTransform>().pos.y;
                lines[1].color = sf::Color::Black;
                m_game->window().draw(lines);
                dot.setPosition(e->getComponent<CFollowPlayer>().home.x, e->getComponent<CFollowPlayer>().home.y);
                m_game->window().draw(dot);
            }
        }
    }

    if (m_drawGrid)
    {
        Vec2 m_gridSize(64, 64);
        float leftX = m_game->window().getView().getCenter().x - width() / 2;
        float rightX = leftX + width() + m_gridSize.x;
        float nextGridX = leftX - ((int)leftX % (int)m_gridSize.x);

        for (float x = nextGridX; x < rightX; x += m_gridSize.x)
        {
            drawLine(Vec2(x, 0), Vec2(x, height()));
        }

        for (float y = 0; y < height(); y += m_gridSize.y)
        {
            drawLine(Vec2(leftX, height() - y), Vec2(rightX, height() - y));

            for (float x = nextGridX; x < rightX; x += m_gridSize.x)
            {
                std::string xCell = std::to_string((int)x / (int)m_gridSize.x);
                std::string yCell = std::to_string((int)y / (int)m_gridSize.y);
                m_gridText.setString("(" + xCell + "," + yCell + ")");
                m_gridText.setPosition(x + 3, height() - y - m_gridSize.y + 2);
                m_game->window().draw(m_gridText);
            }
        }
    }
    m_game->window().display();
}

void Scene_World::sDoAction(const Action& action)
{
    if (action.type() == "START")
    {
        if (action.name() == "TOGGLE_TEXTURE") { m_drawTextures = !m_drawTextures; }
        else if (action.name() == "TOGGLE_COLLISION") { m_drawCollision = !m_drawCollision; }
        else if (action.name() == "TOGGLE_GRID") { m_drawGrid = !m_drawGrid; }
        else if (action.name() == "PAUSE") { openSettings(); }
        else if (action.name() == "QUIT") { onEnd(); }

        else if (action.name() == "LEFT") { m_player->getComponent<CInput>().left = true; }
        else if (action.name() == "RIGHT") { m_player->getComponent<CInput>().right = true; }
        else if (action.name() == "UP") { m_player->getComponent<CInput>().up = true; }
        else if (action.name() == "DOWN") { m_player->getComponent<CInput>().down = true; }

        else if (action.name() == "ATTACK") { m_player->getComponent<CInput>().attack = true; }
    }
    else if (action.type() == "END")
    {
        if (action.name() == "LEFT") { m_player->getComponent<CInput>().left = false; }
        else if (action.name() == "RIGHT") { m_player->getComponent<CInput>().right = false; }
        else if (action.name() == "UP") { m_player->getComponent<CInput>().up = false; }
        else if (action.name() == "ATTACK")
        {
            m_player->getComponent<CInput>().attack = false;
        }
    }
}
