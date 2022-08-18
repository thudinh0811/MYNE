#include "Scene_World.h"
#include "Scene_End.h"
#include "Scene_Play.h"
#include "Scene_Settings.h"
#include "Scene_Editor.h"
#include "Physics.h"
#include "Assets.h"
#include "GameEngine.h"
#include "Components.h"
#include "Settings.h"
#include "Pathfinding.h"

Scene_Play::Scene_Play(GameEngine* game, const std::string& levelPath)
	: Scene(game)
	, m_levelPath(levelPath)
{
	init(m_levelPath);
}

void Scene_Play::init(const std::string& levelPath)
{
	loadLevel(levelPath);
    
    bindActions();

    m_gridText.setCharacterSize(12);
    m_gridText.setFont(m_game->assets().getFont("DPComic"));

    m_healthBar = m_game->assets().getAnimation("HealthBar");
    m_playerInventory.setEngineReference(m_game);

    m_invBox.setSize(sf::Vector2f(64, 64));
    m_invBox.setOutlineColor(sf::Color::White);
    m_invBox.setOutlineThickness(3);
    m_invBox.setOrigin(32, 32);
    m_invBox.setFillColor(sf::Color(0, 0, 0, 0));

    m_minimap.setViewport(sf::FloatRect(0.65f, 0.0f, 0.35f, 0.10f));
    m_minimap.reset(sf::FloatRect(0, 0, 100 * 64, 11*64));

    auto winsize = m_game->window().getSize();
    m_screenTexture.create(winsize.x, winsize.y);
    m_screen.setTexture(m_screenTexture);

	spawnPlayer();
    auto hand = m_entityManager.addEntity("hand");
    hand->addComponent<CTransform>();
}

void Scene_Play::bindActions()
{
    registerAction(sf::Keyboard::Escape, "QUIT");
    registerAction(sf::Keyboard::P, "PAUSE");
    registerAction(sf::Keyboard::N, "TOGGLE_NAV");
    registerAction(sf::Keyboard::T, "TOGGLE_TEXTURE");      // Toggle drawing (T)extures
    registerAction(sf::Keyboard::G, "TOGGLE_GRID");    // Toggle drawing grid lines
    registerAction(sf::Keyboard::C, "TOGGLE_COLLISION");    // Toggle drawing (C)ollision Boxes
    registerAction(sf::Keyboard::LShift, "LSHIFT");
    registerAction(sf::Keyboard::O, "EDIT");
    
    for (const auto& [key, value] : Settings::keyMap)
    {
        registerAction(key, value);
    }

}

void Scene_Play::loadLevel(const std::string& filename)
{
    m_entityManager = EntityManager();
    std::fstream fin(filename);

    std::string type;
    while (fin.good())
    {
        fin >> type;
        if (type == "Player")
        {
            fin >> m_playerConfig.X >> m_playerConfig.Y >> m_playerConfig.CX >> m_playerConfig.CY
                >> m_playerConfig.JUMP >> m_playerConfig.SPEED >> m_playerConfig.MAXSPEED >> m_playerConfig.GRAVITY >> m_playerConfig.HEALTH;
        }

        if (type == "Tile" || type == "Exit" || type == "MTile")
        {
            std::string anim;
            float gx, gy;
            int bm, bv;
            fin >> anim >> gx >> gy >> bm >> bv;
            auto tile = m_entityManager.addEntity("tile");
            tile->addComponent<CAnimation>(m_game->assets().getAnimation(anim), true);
            tile->addComponent<CTransform>(getPosition(gx, gy));
            tile->addComponent<CBoundingBox>(m_game->assets().getAnimation(anim).getSize(), bm, bv);

            if (type == "Exit")
            {
                tile->addComponent<CExit>();
            }

            if (type == "MTile")
            {
                float s;
                int n;
                int x, y;
                std::vector<Vec2> points;
                fin >> s >> n;
                for (int i = 0; i < n; i++)
                {
                    fin >> x >> y;
                    points.push_back(getPosition(x, y));
                }

                tile->addComponent<CPatrol>(points, s);
            }
        }

        if (type == "Mob")
        {
            std::string anim, ai;
            float gx, gy;
            int bm, bv, h, d, grav;
            fin >> anim >> gx >> gy >> bm >> bv >> grav >> h >> d >> ai;
            auto npc = m_entityManager.addEntity("mob");
            npc->addComponent<CAnimation>(m_game->assets().getAnimation(anim), true);
            npc->addComponent<CTransform>(getPosition(gx, gy));
            npc->addComponent<CBoundingBox>(m_game->assets().getAnimation(anim).getSize(), bm, bv);
            npc->addComponent<CDamage>(d);
            npc->addComponent<CHealth>(h, h);
            if (grav)
            {
                npc->addComponent<CGravity>(m_playerConfig.GRAVITY);
            }

            if (ai == "Boss")
            {
                std::string pr;
                float as, ar;
                fin >> pr >> as >> ar;
                npc->addComponent<CShootPlayer>(pr, as, ar);
            }

            if (ai == "Follow")
            {
                float s, d;
                fin >> s >> d;
                npc->addComponent<CFollowPlayer>(getPosition(gx, gy), s, d);
            }
            else if (ai == "Patrol" || ai == "Boss")
            {
                float s;
                int n;
                float x, y;
                std::vector<Vec2> points;
                fin >> s >> n;
                for (int i = 0; i < n; i++)
                {
                    fin >> x >> y;
                    points.push_back(getPosition(x, y));
                }

                npc->addComponent<CPatrol>(points, s);
            }
        }

        if (type == "Item")
        {
            std::string anim;
            float gx, gy;
            fin >> anim >> gx >> gy;
            auto item = m_entityManager.addEntity("item");
            item->addComponent<CTransform>(getPosition(gx, gy));
            item->addComponent<CBoundingBox>(m_game->assets().getAnimation(anim).getSize(), 0, 0);
            item->addComponent<CAnimation>(m_game->assets().getAnimation(anim), true);
       }

        if (type == "Dec")
        {
            std::string anim;
            float gx, gy;
            fin >> anim >> gx >> gy;
            auto dec = m_entityManager.addEntity("dec");
            dec->addComponent<CTransform>(getPosition(gx, gy));
            dec->addComponent<CBoundingBox>(m_game->assets().getAnimation(anim).getSize(), 0, 0);
            dec->addComponent<CAnimation>(m_game->assets().getAnimation(anim), true);

        }

        if (type == "Nav")
        {
            std::string nm;
            fin >> nm;
            loadNavMap(nm);
        }

        if (type == "ID")
        {
            fin >> m_levelID;
        }

        if (type == "Music")
        {
            fin >> m_levelMusic;
            m_game->playMusic(m_levelMusic);
        }

        if (type == "Parallax")
        {
            std::string anim1, anim2, anim3, anim4;
            fin >> anim1 >> anim2 >> anim3 >> anim4;
            Animation anims[] = { m_game->assets().getAnimation(anim1), m_game->assets().getAnimation(anim2), m_game->assets().getAnimation(anim3), m_game->assets().getAnimation(anim4) };
            float strengths[] = { 0.9f, 0.7f, 0.3f, -0.2f };
            m_parallax = Parallax(anims, strengths, Vec2(0, m_game->window().getSize().y));
        }
    }
}

void Scene_Play::loadNavMap(const std::string& filename)
{
    std::fstream fin(filename);

    while (fin.good())
    {
        std::string line;
        std::getline(fin, line);
        std::stringstream ss(line);
        float x, y;
        size_t n, t;
        ss >> x >> y >> n;
        NavPoint p(Vec2(x, y));
        for (size_t i = 0; i < n; i++)
        {
            ss >> t;
            p.connections.push_back(t);
        }
        m_navMap.push_back(p);
    }

}

void Scene_Play::spawnPlayer()
{
    m_player = m_entityManager.addEntity("player");
    m_player->addComponent<CTransform>(getPosition(m_playerConfig.X, m_playerConfig.Y));
    m_player->addComponent<CAnimation>(m_game->assets().getAnimation("Stand"), true);
    m_player->addComponent<CBoundingBox>(Vec2(m_playerConfig.CX, m_playerConfig.CY), true, false);
    m_player->addComponent<CInput>();
    m_player->addComponent<CHealth>(m_playerConfig.HEALTH, m_playerConfig.HEALTH);
    m_player->addComponent<CGravity>(m_playerConfig.GRAVITY);
}

void Scene_Play::onComplete()
{
    std::fstream file("pg.txt");
    bool c[3];
    file >> c[0] >> c[1] >> c[2];
    file.close();
    c[m_levelID] = true;
    file.open("pg.txt", std::fstream::out | std::fstream::trunc);
    file << c[0] << ' ' << c[1] << ' ' << c[2];
    file.close();
    m_game->playSound("LevelComplete");

    if (m_levelID == 2)
    {
		m_game->assets().getMusic(m_levelMusic).stop();
        m_game->changeScene("END", std::make_shared<Scene_End>(m_game, "credits.txt"), true);
    }
    else
    {
        onEnd();
    }
}

void Scene_Play::openSettings()
{
    m_game->changeScene("SETTINGS", std::make_shared<Scene_Settings>(m_game, "PLAY"));
}

void Scene_Play::onEnd()
{
	m_game->assets().getMusic(m_levelMusic).stop();
    m_game->playMusic("MainTheme");
	m_game->changeScene("WORLD", std::make_shared<Scene_World>(m_game, "world_view.txt"), true);
    auto view = m_game->window().getDefaultView();
    view.setRotation(0);
    m_game->window().setView(view);
}

void Scene_Play::onEdit()
{
    if (m_shift)
    {
		m_game->assets().getMusic(m_levelMusic).stop();
        m_game->changeScene("EDIT", std::make_shared<Scene_Editor>(m_game, m_levelPath), true);
    }
}

void Scene_Play::loadShader(const std::string& shaderPath)
{
    m_shader.loadFromFile(shaderPath, sf::Shader::Fragment);
    m_shader.setUniform("time", (float)m_currentFrame);
}

bool Scene_Play::canSee(std::shared_ptr<Entity> target, std::shared_ptr<Entity> looker)
{
    Vec2 a = looker->getComponent<CTransform>().pos;
    Vec2 b = target->getComponent<CTransform>().pos;

    for (auto entity : m_entityManager.getEntities())
    {
        if (entity->id() == target->id() || entity->id() == looker->id())
        {
            continue;
        }
        if (entity->hasComponent<CBoundingBox>() && entity->getComponent<CBoundingBox>().blockVision)
        {
            if (Physics::EntityIntersect(a, b, entity))
            {
                return false;
            }
        }
    }
    return true;
}

bool Scene_Play::canSee(Vec2 target, Vec2 looker)
{
    for (auto entity : m_entityManager.getEntities())
    {
        if (entity->hasComponent<CBoundingBox>() && entity->getComponent<CBoundingBox>().blockVision)
        {
            if (Physics::EntityIntersect(looker, target, entity))
            {
                return false;
            }
        }
    }
    return true;
}


bool Scene_Play::canSee(std::shared_ptr<Entity> target, Vec2 lookerPos)
{
    Vec2 b = target->getComponent<CTransform>().pos;

    for (auto entity : m_entityManager.getEntities())
    {
        if (entity->id() == target->id())
        {
            continue;
        }
        if (entity->hasComponent<CBoundingBox>() && entity->getComponent<CBoundingBox>().blockVision)
        {
            if (Physics::EntityIntersect(lookerPos, b, entity))
            {
                return false;
            }
        }
    }
    return true;
}


void Scene_Play::update()
{
    if (!m_paused)
    {
        m_entityManager.update();
        sAI();
        sInput();
        sInventory();
        sState();
        sMovement();
        sCollision();
        sCamera();
        sAnimation();
        m_currentFrame++;
    }
}

void Scene_Play::sMovement()
{
    for (auto entity : m_entityManager.getEntities())
    {
        if (entity->hasComponent<CTransform>())
        {
            auto& trans = entity->getComponent<CTransform>();
            trans.prevPos = trans.pos;
            if (entity->hasComponent<CGravity>())
            {
                trans.velocity.y += entity->getComponent<CGravity>().gravity;
            }

            trans.pos += trans.velocity;
        }
    }
}

void Scene_Play::sInput()
{
    for (auto entity : m_entityManager.getEntities())
    {
        if (entity->hasComponent<CInput>())
        {
            auto& trans = entity->getComponent<CTransform>();
            auto& input = entity->getComponent<CInput>();
            if (input.up & entity->getComponent<CGravity>().canJump)
            {
                trans.velocity.y = m_playerConfig.JUMP;
                m_game->playSound("Jump");
            }
            else if (!input.up && trans.velocity.y < 0)
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
    }
}

void Scene_Play::sAI()
{
    for (auto entity : m_entityManager.getEntities("mob"))
    {

		auto& transform = entity->getComponent<CTransform>();
        entity->addComponent<CShader>("Breath");

        if (entity->hasComponent<CKnock>())
        {
            auto target = transform.pos - m_player->getComponent<CTransform>().pos;
            moveToTarget(transform.pos + target, entity, 5);
        }
        else if (entity->hasComponent<CPatrol>())
        {
            auto& patrol = entity->getComponent<CPatrol>();
            Vec2 target = patrol.positions[patrol.currentPosition];
            if (transform.pos.dist(target) < 40)
            {
                patrol.currentPosition++;
                patrol.currentPosition %= patrol.positions.size();
                transform.velocity.x = 0;
            }
            else
            {
                moveToTarget(target, entity, patrol.speed);
            }
        }
        else if (entity->hasComponent<CFollowPlayer>())
        {
            auto& follow = entity->getComponent<CFollowPlayer>();
            Vec2 target = m_player->getComponent<CTransform>().pos;
            target = follow.home.dist(target) < follow.followDist ? target : follow.home;
            float jump = m_playerConfig.SPEED / follow.speed * m_playerConfig.JUMP;

            if (!entity->hasComponent<CGravity>())
            {
                if (canSee(entity, m_player) && follow.followDist > target.dist(transform.pos))
                {
                    moveToTarget(target, entity, follow.speed);
                }
                else
                {
                    transform.velocity = Vec2(0, 0);
                }
            }
            else if (transform.pos.dist(target) < 10)
            {
                transform.velocity.x = 0;
            }
            else
            {
				auto lpos = entity->getComponent<CGravity>().canJump ? transform.pos : follow.lastGroundPos;

				Vec2 velTarget;
				auto targets = Pathfinding::findPath(lpos, target, m_navMap, this);
				if (targets.size() == 0)
				{
					transform.velocity = Vec2();
                    velTarget = transform.pos;
				}
				else if (targets.size() == 1)
				{
                    velTarget = getPosition(targets[0].x, targets[0].y);
					moveToTarget(getPosition(targets[0].x, targets[0].y), entity, follow.speed);
				}
				else
				{
					auto target0 = getPosition(targets[0].x, targets[0].y);
					auto target1 = getPosition(targets[1].x, targets[1].y);

					if (transform.pos.dist(target0) < 70 || abs(transform.pos.y - target0.y) > abs(transform.pos.y - target1.y))
					{
						velTarget = target1;
						moveToTarget(target1, entity, follow.speed);
					}
					else
					{
						velTarget = target0;
						moveToTarget(target0, entity, follow.speed);
					}
				}

                Vec2 vel = (velTarget - transform.pos);
				if (vel.y < -63 && entity->getComponent<CGravity>().canJump)
				{
					transform.velocity.y = jump;
				}

				float mag = transform.velocity.dist(Vec2());
				if (mag > m_playerConfig.MAXSPEED + 1)
				{
					transform.velocity.y = ((transform.velocity / mag) * (m_playerConfig.MAXSPEED + 1)).y;
				}
            }

            if (entity->getComponent<CGravity>().canJump) { follow.lastGroundPos = transform.pos; }
        }

        if (entity->hasComponent<CShootPlayer>())
        {
            auto& shoot = entity->getComponent<CShootPlayer>();
            if (!entity->hasComponent<CWeaponReset>() && transform.pos.dist(m_player->getComponent<CTransform>().pos) < shoot.attackRange && canSee(m_player, entity))
            {
                auto poison = m_entityManager.addEntity("poison");
				poison->addComponent<CAnimation>(m_game->assets().getAnimation(shoot.projectile), true);
				poison->addComponent<CBoundingBox>(m_game->assets().getAnimation(shoot.projectile).getSize(), false, false);
				poison->addComponent<CTransform>(transform.pos);
				poison->addComponent<CLifeSpan>(60);
				
                poison->getComponent<CTransform>().velocity = (m_player->getComponent<CTransform>().pos - transform.pos);
                float mag = poison->getComponent<CTransform>().velocity.dist(Vec2(0, 0));
                if (mag > 0)
                {
					poison->getComponent<CTransform>().velocity = (poison->getComponent<CTransform>().velocity / mag) * 12;
                }

                poison->getComponent<CTransform>().angle = atan2f(poison->getComponent<CTransform>().velocity.y, poison->getComponent<CTransform>().velocity.x) * 180 / 3.141593f;
                entity->addComponent<CWeaponReset>(shoot.attackSpeed);
            }
        }

        if (!entity->hasComponent<CKnock>())
        {
            if (transform.velocity.x >= 0) { transform.scale.x = -1; }
            else { transform.scale.x = 1; }
        }
    }

    for (auto entity : m_entityManager.getEntities("tile"))
    {
        if (entity->hasComponent<CPatrol>())
        {
            auto& transform = entity->getComponent<CTransform>();
            auto& patrol = entity->getComponent<CPatrol>();
            Vec2 target = patrol.positions[patrol.currentPosition];
            if (transform.pos.dist(target) < 9)
            {
                patrol.currentPosition++;
                patrol.currentPosition %= patrol.positions.size();
                transform.velocity.x = 0;
            }
            else
            {
                moveToTarget(target, entity, patrol.speed);
            }
        }

    }
}

void Scene_Play::moveToTarget(Vec2 target, std::shared_ptr<Entity> entity, float speed)
{
    if (entity->hasComponent<CGravity>())
    {
        entity->getComponent<CTransform>().velocity.x = (target - entity->getComponent<CTransform>().pos).x;
        float mag = entity->getComponent<CTransform>().velocity.dist(Vec2(0, 0));
        if (mag > 0)
        {
            entity->getComponent<CTransform>().velocity.x = (entity->getComponent<CTransform>().velocity / mag * speed).x;
        }
    }
    else
    {
        entity->getComponent<CTransform>().velocity = (target - entity->getComponent<CTransform>().pos);
        float mag = entity->getComponent<CTransform>().velocity.dist(Vec2(0, 0));
        if (mag > 0)
        {
            entity->getComponent<CTransform>().velocity = (entity->getComponent<CTransform>().velocity / mag * speed);
        }
    }
}

void Scene_Play::sState()
{
    for (auto entity : m_entityManager.getEntities())
    {
        if (entity->hasComponent<CLifeSpan>())
        {
            entity->getComponent<CLifeSpan>().lifeleft--;
            if (entity->getComponent<CLifeSpan>().lifeleft < 1)
            {
                entity->destroy();
            }
        }

        if (entity->hasComponent<CStatusEffect>())
        {
            auto& eff = entity->getComponent<CStatusEffect>();
            if (eff.effect == "SP")
            {
                entity->addComponent<CShader>("Speed");
                entity->getComponent<CTransform>().velocity.x *= 1.7f;
                entity->getComponent<CGravity>().gravity = m_playerConfig.GRAVITY * 0.8f;
            }
            else if (eff.effect == "HP")
            {
                entity->addComponent<CShader>("Regen");
                entity->getComponent<CHealth>().current = entity->getComponent<CHealth>().max;
            }
            else if (eff.effect == "DMG")
            {
                entity->addComponent<CShader>("DamageBoost");
                entity->getComponent<CDamage>().damage *= 2;
            }
            else if (eff.effect == "Mirror")
            {
                m_flipview = true;
            }

            eff.duration--;
            if (eff.duration < 1)
            {
                entity->removeComponent<CShader>();
                entity->getComponent<CGravity>().gravity = m_playerConfig.GRAVITY;
                entity->removeComponent<CStatusEffect>();
                m_flipview = false;
            }
        }

        if (entity->hasComponent<CInvincibility>())
        {
            entity->addComponent<CShader>("Damaged");
            entity->getComponent<CInvincibility>().iframes--;
            if (entity->getComponent<CInvincibility>().iframes < 1)
            {
                entity->removeComponent<CInvincibility>();
                entity->removeComponent<CShader>();
            }
        }
        if (entity->hasComponent<CHealth>() && entity->getComponent<CHealth>().current == 0)
        {
            entity->destroy();
            if (entity->id() == m_player->id())
            {
                m_player->removeComponent<CStatusEffect>();
                m_flipview = false;
                onEnd();
            }
            else
            {
                m_game->playSound("EnemyDeath");
            }
        }
        if (entity->hasComponent<CKnock>())
        {
            entity->addComponent<CShader>("Damaged");
		    entity->getComponent<CKnock>().frames--;
            if (entity->getComponent<CKnock>().frames < 1)
            {
                entity->removeComponent<CKnock>();
                entity->removeComponent<CShader>();
            }
  
        }

        if (entity->hasComponent<CWeaponReset>())
        {
            entity->getComponent<CWeaponReset>().frames--;
            if (entity->getComponent<CWeaponReset>().frames < 1)
            {
                entity->removeComponent<CWeaponReset>();
                if (!(entity->tag() == "mob"))
                {
                    entity->removeComponent<CBoundingBox>();
                }
                entity->getComponent<CTransform>().scale.y = 1;
            }
        }
    }


    bool done = true;
    for (auto ent : m_entityManager.getEntities("mob"))
    {
        if(ent->hasComponent<CShootPlayer>()) 
        {
            done = false;
            break;
        }
    }

    m_bossClear = done;
    if(done)
    {
        for (auto tile : m_entityManager.getEntities("tile"))
        {
            if (tile->hasComponent<CExit>())
            {
                tile->destroy();
            }
        }
    }

	auto& playerHealth = m_player->getComponent<CHealth>();
    m_healthBar.getSprite().setTextureRect(sf::IntRect(0, 0, 35 + ((m_healthBar.getSize().x - 70) / playerHealth.max * playerHealth.current), m_healthBar.getSize().y));
}

void Scene_Play::sInventory()
{
	auto& playerTrans = m_player->getComponent<CTransform>();
    auto held = m_playerInventory.getItem();
    auto& hand = m_entityManager.getEntities("hand")[0];
    auto& input = m_player->getComponent<CInput>();
    if (input.next)
    {
        held = m_playerInventory.getNextItem();
        input.next = false;
    }
    if (input.last)
    {
        held = m_playerInventory.getLastItem();
        input.last = false;
    }

	hand->getComponent<CTransform>().pos = playerTrans.pos + (playerTrans.scale.x > 0 ? Vec2(32, 0) : Vec2(-32, 0));
    hand->getComponent<CTransform>().scale.x = playerTrans.scale.x;
    if (held != nullptr && !hand->hasComponent<CWeaponReset>())
    {
		hand->getComponent<CWeaponReset>().frames = held->getComponent<CItem>().cooldown;
		if (!hand->hasComponent<CAnimation>()
			|| held->getComponent<CAnimation>().animation.getName() != hand->getComponent<CAnimation>().animation.getName())
		{
			hand->addComponent<CAnimation>(m_game->assets().getAnimation(held->getComponent<CAnimation>().animation.getName()), true);
		}

        if (held->hasComponent<CStatusEffect>())
        {
            hand->addComponent<CStatusEffect>(held->getComponent<CStatusEffect>().effect, held->getComponent<CStatusEffect>().duration);
        }
        else
        {
            hand->removeComponent<CStatusEffect>();
            hand->removeComponent<CShader>();
        }

		if (input.attack && m_canAttack)
		{
            m_canAttack = false;
            auto f = m_playerInventory.useItem(held->tag());
            f(m_player, hand, m_entityManager, m_game);
		}
    }
    
    if (held != nullptr && held->hasComponent<CDamage>())
    {
		m_player->addComponent<CDamage>(held->getComponent<CDamage>().damage);
    }
	else
	{
		m_player->removeComponent<CDamage>();
	}
}


void Scene_Play::sCollision()
{
    auto& playerTrans = m_player->getComponent<CTransform>();
    auto& playerInput = m_player->getComponent<CInput>();
    m_player->getComponent<CGravity>().canJump = false;

    for (auto tile : m_entityManager.getEntities("tile"))
    {
        Vec2 overlap = Physics::GetOverlap(m_player, tile);
        if (tile->getComponent<CBoundingBox>().blockMove && overlap.x > 0 && overlap.y > 0)
        {
            Vec2 prevOverlap = Physics::GetPreviousOverlap(m_player, tile);
            if (prevOverlap.x > 0 || (prevOverlap.x > 0 && prevOverlap.y > 0))
            {
                bool above = playerTrans.pos.y < tile->getComponent<CTransform>().pos.y;
                playerTrans.pos.y -= above ? overlap.y : -overlap.y;
                m_player->getComponent<CGravity>().canJump = above;
                playerTrans.velocity.y = 0;

				if (above && tile->hasComponent<CPatrol>())
				{
					playerTrans.pos.x += tile->getComponent<CTransform>().velocity.x;
				}
            }
            else if (prevOverlap.y > 0)
            {
                playerTrans.pos.x -= playerTrans.pos.x < tile->getComponent<CTransform>().pos.x ? overlap.x : -overlap.x;
            }

        }

        for (auto bullet : m_entityManager.getEntities("bullet"))
        {
			Vec2 overlap = Physics::GetOverlap(tile, bullet);
			if(overlap.x > 0 && overlap.y > 0)
			{
                bullet->destroy();
			}
        }
        for (auto bullet : m_entityManager.getEntities("poison"))
        {
			Vec2 overlap = Physics::GetOverlap(tile, bullet);
			if(overlap.x > 0 && overlap.y > 0)
			{
                bullet->destroy();
			}
        }
    }

	for (auto mob : m_entityManager.getEntities("mob"))
	{
		auto& trans = mob->getComponent<CTransform>();
		mob->getComponent<CGravity>().canJump = false;

		for (auto tile : m_entityManager.getEntities("tile"))
        {
            Vec2 overlap = Physics::GetOverlap(mob, tile);
            if (tile->getComponent<CBoundingBox>().blockMove && overlap.x > 0 && overlap.y > 0)
            {
                Vec2 prevOverlap = Physics::GetPreviousOverlap(mob, tile);
                if (prevOverlap.x > 0 || (prevOverlap.x > 0 && prevOverlap.y > 0))
                {
                    bool above = trans.pos.y < tile->getComponent<CTransform>().pos.y;
                    trans.pos.y -= above ? overlap.y : -overlap.y;
                    trans.velocity.y = 0;
                    mob->getComponent<CGravity>().canJump = above;
                }
                else if (prevOverlap.y > 0)
                {
                    trans.pos.x -= trans.pos.x < tile->getComponent<CTransform>().pos.x ? overlap.x : -overlap.x;
                }
            }
        }

        for (auto hand : m_entityManager.getEntities("hand"))
        {
            auto& handTrans = hand->getComponent<CTransform>();
			Vec2 overlap = Physics::GetOverlap(mob, hand);
			if(overlap.x > 0 && overlap.y > 0)
			{
				if (hand->getComponent<CBoundingBox>().blockMove)
				{
					Vec2 prevOverlap = Physics::GetPreviousOverlap(mob, hand);
					if (prevOverlap.y > 0)
					{
						bool left = trans.pos.x < handTrans.pos.x;
						trans.pos.x -= left ? overlap.x : -overlap.x;
						trans.velocity.x = 0;
					}
					else
					{
						bool above = trans.pos.y < handTrans.pos.y;
						trans.pos.y -= above ? overlap.y : -overlap.y;
						trans.velocity.y = 0;
					}
				}

				if (m_player->hasComponent<CDamage>())
				{
					mob->getComponent<CHealth>().current -= m_player->getComponent<CDamage>().damage * (3 - (int)Settings::difficulty);
					mob->getComponent<CHealth>().current = fmax(mob->getComponent<CHealth>().current, 0);
                    mob->addComponent<CKnock>(5 * m_player->getComponent<CDamage>().damage);
                    hand->removeComponent<CBoundingBox>();
					m_game->playSound("Hit");
				}
			}
        }

        for (auto bullet : m_entityManager.getEntities("bullet"))
        {
            auto& bulletTrans = bullet->getComponent<CTransform>();
			Vec2 overlap = Physics::GetOverlap(mob, bullet);
			if(overlap.x > 0 && overlap.y > 0)
			{
				if (m_player->hasComponent<CDamage>())
				{
					mob->getComponent<CHealth>().current -= m_player->getComponent<CDamage>().damage * (3 - (int)Settings::difficulty);
					mob->getComponent<CHealth>().current = fmax(mob->getComponent<CHealth>().current, 0);
                    mob->addComponent<CKnock>(15 * bullet->getComponent<CDamage>().damage);
					m_game->playSound("Hit");
				}
				bullet->destroy();
			}
        }

        Vec2 overlap = Physics::GetOverlap(mob, m_player);
        if(overlap.x > 0 && overlap.y > 0)
        {
            if (mob->getComponent<CBoundingBox>().blockMove)
            {
                Vec2 prevOverlap = Physics::GetPreviousOverlap(mob, m_player);
                if (prevOverlap.y > 0)
                {
                    bool left = trans.pos.x > playerTrans.pos.x;
                    playerTrans.pos.x -= left ? overlap.x : -overlap.x;
                    playerTrans.velocity.x = 0;
                    trans.velocity.x = 0;
                }
                else
                {
                    bool above = trans.pos.y > playerTrans.pos.y;
                    playerTrans.pos.y -= above ? overlap.y : -overlap.y;
                    playerTrans.velocity.y = 0;
                    trans.velocity.y = 0;
                }
            }

            if (!m_player->hasComponent<CInvincibility>())
            {
                m_player->getComponent<CHealth>().current -= mob->getComponent<CDamage>().damage * (1 + (int)Settings::difficulty);
                m_player->getComponent<CHealth>().current = fmax(m_player->getComponent<CHealth>().current, 0);
                m_player->addComponent<CInvincibility>(30);
                m_game->playSound("Hit");
            }
        }
    }

    for (auto item : m_entityManager.getEntities("item"))
    {
        Vec2 overlap = Physics::GetOverlap(m_player, item);
        if (overlap.x > 0 && overlap.y > 0)
        {
            item->destroy();
            m_playerInventory.addItem(item->getComponent<CAnimation>().animation.getName());
            m_game->playSound("Pickup");
        }
    }

	for (auto bullet : m_entityManager.getEntities("poison"))
	{
		Vec2 overlap = Physics::GetOverlap(m_player, bullet);
		if(overlap.x > 0 && overlap.y > 0)
		{
			m_player->getComponent<CHealth>().current -= bullet->getComponent<CDamage>().damage * (1 + (int)Settings::difficulty);
			m_player->getComponent<CHealth>().current = fmax(m_player->getComponent<CHealth>().current, 0);
			m_player->addComponent<CInvincibility>(30);
			m_game->playSound("Hit");
            if (bullet->getComponent<CAnimation>().animation.getName() == "Poison")
            {
                m_player->addComponent<CStatusEffect>("Mirror", 120);
            }

			bullet->destroy();
		}
	}


    if (playerTrans.pos.x < m_player->getComponent<CBoundingBox>().halfSize.x)
    {
        playerTrans.pos.x = m_player->getComponent<CBoundingBox>().halfSize.x;
    }
    else if (playerTrans.pos.x > getPosition(105, 1).x)
    {
        onComplete();
    }
}

void Scene_Play::sCamera()
{
    sf::View view = m_game->window().getDefaultView();
    view.setCenter(fmax(m_player->getComponent<CTransform>().pos.x, m_game->window().getSize().x / 2), view.getCenter().y);
    auto xdiff = view.getCenter().x - m_game->window().getSize().x / 2;

    if (m_flipview)
    {
        view.setSize(-((float)m_game->window().getSize().x), view.getSize().y);
    }
    else
    {
        view.setSize(m_game->window().getSize().x, view.getSize().y);
    }

    m_game->window().setView(view);

	m_healthBar.getSprite().setPosition(150 + xdiff, 30);
    m_invBox.setPosition(170 + xdiff, 70);

    m_parallax.update(xdiff);
}

void Scene_Play::sAnimation()
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
        if (entity->hasComponent<CAnimation>())
        {
            auto& anim = entity->getComponent<CAnimation>();
            anim.animation.update();
            if (!anim.repeat && anim.animation.hasEnded())
            {
                entity->destroy();
            }
        }
    }
}

void Scene_Play::sRender()
{

    m_game->window().clear(sf::Color(0, 0, 0));
                          
    // draw all Entity textures / animations
    if (m_drawTextures)
    {
        m_parallax.drawBack(m_game->window());

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

                bool hasShader = e->hasComponent<CShader>();
                if (hasShader)
                {
                    loadShader(m_game->assets().getShader(e->getComponent<CShader>().shader));
                }

				m_game->window().draw(animation.getSprite(), hasShader ? &m_shader : nullptr);
            }
        }
        
        m_parallax.drawFront(m_game->window());

		if (m_levelID == 2 && !m_bossClear)
		{
			m_screenTexture.update(m_game->window());
            auto& v = m_game->window().getView();
            m_game->window().setView(m_game->window().getDefaultView());
			m_game->window().clear();
			m_shader.loadFromFile(m_game->assets().getShader("Lighting"), sf::Shader::Fragment);
			m_shader.setUniform("time", (float)m_currentFrame);
			m_game->window().draw(m_screen, &m_shader);
            m_game->window().setView(v);
		}

        m_game->window().draw(m_healthBar.getSprite());
        m_game->window().draw(m_invBox);
        auto last = m_playerInventory.peekLastItem();
        if (last != nullptr)
        {
            m_invPeek[0] = last->getComponent<CAnimation>().animation;
			m_invPeek[0].getSprite().setPosition(m_invBox.getPosition() - sf::Vector2f(70, 0));
            m_game->window().draw(m_invPeek[0].getSprite());
        }
        auto current = m_playerInventory.getItem();
        if(current != nullptr)
        {
            m_invPeek[1] = current->getComponent<CAnimation>().animation;
			m_invPeek[1].getSprite().setPosition(m_invBox.getPosition());
			m_game->window().draw(m_invPeek[1].getSprite());
        }
        auto next = m_playerInventory.peekNextItem();
        if (next != nullptr)
        {
            m_invPeek[2] = next->getComponent<CAnimation>().animation;
			m_invPeek[2].getSprite().setPosition(m_invBox.getPosition() + sf::Vector2f(70, 0));
            m_game->window().draw(m_invPeek[2].getSprite());
        }
    }

    // draw all Entity collision bounding boxes with a rectangleshape
    if (m_drawCollision)
    {
        sf::CircleShape dot(4);
        dot.setFillColor(sf::Color::White);
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
					dot.setFillColor(sf::Color::White);
					dot.setRadius(4);
					dot.setOrigin(4, 4);
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
				dot.setFillColor(sf::Color::White);
                dot.setRadius(4);
                dot.setOrigin(4, 4);
                dot.setPosition(e->getComponent<CFollowPlayer>().home.x, e->getComponent<CFollowPlayer>().home.y);
                m_game->window().draw(dot);
                dot.setRadius(e->getComponent<CFollowPlayer>().followDist);
                dot.setFillColor(sf::Color(0, 0, 0, 0));
                dot.setOutlineColor(sf::Color::White);
                dot.setOutlineThickness(3);
                dot.setOrigin(dot.getRadius(), dot.getRadius());
                m_game->window().draw(dot);
            }
        }
    }

    if (m_drawNav)
    {
		sf::CircleShape dot(4);
        dot.setOrigin(4, 4);
        dot.setFillColor(sf::Color::Yellow);

        for (auto point : m_navMap)
        {
            sf::VertexArray lines(sf::Lines, point.connections.size() * 2);
            Vec2 pos = getPosition(point.position.x, point.position.y);
            dot.setPosition(pos.x, pos.y);
            for (size_t i = 0; i < point.connections.size(); i++)
            {
                lines[i * 2].position.x = pos.x;
                lines[i * 2].position.y = pos.y;
                lines[i * 2].color = sf::Color::Red;

                auto pos2 = getPosition(m_navMap[point.connections[i]].position.x, m_navMap[point.connections[i]].position.y);
                lines[i * 2 + 1].position.x = pos2.x;
                lines[i * 2 + 1].position.y = pos2.y;
                lines[i * 2 + 1].color = sf::Color::Blue;
                lines[i * 2 + 1].color.a = 20;
            }
            m_game->window().draw(lines);
            m_game->window().draw(dot);
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

    {
        m_game->window().setView(m_minimap);
        m_shader.loadFromFile(m_game->assets().getShader("Solid"), sf::Shader::Fragment);

		m_shader.setUniform("rgba", sf::Glsl::Vec4(0.f, 0.f, 0.f, 0.2f));
        m_game->window().draw(sf::RectangleShape(sf::Vector2f(m_minimap.getSize())), &m_shader);


		m_shader.setUniform("rgba", sf::Glsl::Vec4(1.f, 1.f, 1.f, 1.f));
        for (auto tile : m_entityManager.getEntities("tile"))
        {
            m_game->window().draw(tile->getComponent<CAnimation>().animation.getSprite(), &m_shader);
        }

		m_shader.setUniform("rgba", sf::Glsl::Vec4(1.f, 0.f, 0.f, 1.f));
        for (auto tile : m_entityManager.getEntities("mob"))
        {
            m_game->window().draw(tile->getComponent<CAnimation>().animation.getSprite(), &m_shader);
        }
        
        m_shader.setUniform("rgba", sf::Glsl::Vec4(0.f, 0.f, 1.f, 1.f));
        for (auto tile : m_entityManager.getEntities("item"))
        {
            m_game->window().draw(tile->getComponent<CAnimation>().animation.getSprite(), &m_shader);
        }

		m_shader.setUniform("rgba", sf::Glsl::Vec4(1.f, 0.f, 1.f, 1.f));
        m_game->window().draw(m_player->getComponent<CAnimation>().animation.getSprite(), &m_shader);
    }

    m_game->window().setView(m_game->window().getDefaultView());

    m_game->window().display();
}

void Scene_Play::sDoAction(const Action& action)
{
    if (action.type() == "START")
    {
        if (action.name() == "TOGGLE_TEXTURE") { m_drawTextures = !m_drawTextures; }
        else if (action.name() == "TOGGLE_COLLISION") { m_drawCollision = !m_drawCollision; }
        else if (action.name() == "TOGGLE_GRID") { m_drawGrid = !m_drawGrid; }
        else if (action.name() == "TOGGLE_NAV") { m_drawNav = !m_drawNav; }
        else if (action.name() == "PAUSE") { openSettings(); }
        else if (action.name() == "QUIT") { onEnd(); }
        else if (action.name() == "EDIT") { onEdit(); }
        else if (action.name() == "LSHIFT") { m_shift = true; }

        else if (action.name() == "LEFT") { m_player->getComponent<CInput>().left = true; }
        else if (action.name() == "RIGHT") { m_player->getComponent<CInput>().right = true; }
        else if (action.name() == "UP") { m_player->getComponent<CInput>().up = true; }
        else if (action.name() == "DOWN") { m_player->getComponent<CInput>().down = true; }

        else if (action.name() == "NEXT") { m_player->getComponent<CInput>().next = true; }
        else if (action.name() == "LAST") { m_player->getComponent<CInput>().last = true; }
        else if (action.name() == "USE") { m_player->getComponent<CInput>().attack = true; }
    }
    else if (action.type() == "END")
    {
        if (action.name() == "LEFT") { m_player->getComponent<CInput>().left = false; }
        else if (action.name() == "RIGHT") { m_player->getComponent<CInput>().right = false; }
        else if (action.name() == "UP") { m_player->getComponent<CInput>().up = false; }
        else if (action.name() == "USE") { m_player->getComponent<CInput>().attack = false; m_canAttack = true; }
        else if (action.name() == "LSHIFT") { m_shift = false; }
    }
}
