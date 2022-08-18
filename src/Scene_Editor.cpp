#include "Scene_Editor.h"
#include "GameEngine.h"
#include "Scene_Settings.h"
#include "Scene_World.h"
#include "Settings.h"

std::shared_ptr<Entity> CDrag::attached = nullptr;

Scene_Editor::Scene_Editor(GameEngine* gameEngine, const std::string& levelPath)
    : Scene(gameEngine)
	, m_levelPath(levelPath)

{
	init(m_levelPath);
}

void Scene_Editor::loadNavMap(const std::string& filename)
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


void Scene_Editor::init(const std::string& levelPath)
{
	loadLevel(levelPath);

    bindActions();

    m_gridText.setCharacterSize(12);
    m_gridText.setFont(m_game->assets().getFont("DPComic"));

}

void Scene_Editor::loadLevel(const std::string& filename)
{
    m_entityManager = EntityManager();
    std::fstream fin(filename);

    std::string line;
    while (fin.good())
    {
        std::getline(fin, line);
        std::stringstream ss(line);
        insert(ss);
    }
    fin.close();
}

void Scene_Editor::bindActions()
{
    registerAction(sf::Keyboard::Escape, "QUIT");
    registerAction(sf::Keyboard::P, "PAUSE");
    registerAction(sf::Keyboard::N, "TOGGLE_NAV");
    registerAction(sf::Keyboard::T, "TOGGLE_TEXTURE");      // Toggle drawing (T)extures
    registerAction(sf::Keyboard::G, "TOGGLE_GRID");    // Toggle drawing grid lines
    registerAction(sf::Keyboard::C, "TOGGLE_COLLISION");    // Toggle drawing (C)ollision Boxes
    registerAction(sf::Keyboard::L, "SAVE_TO_FILE");
    
    registerAction(sf::Keyboard::I, "INSERT");

    for (const auto& [key, value] : Settings::keyMap)
    {
        registerAction(key, value);
    }

}

void Scene_Editor::saveToFile()
{
    std::ofstream fout("temp.txt", std::ios::trunc | std::ios::out);

    if (fout.is_open())
    {
        for (auto entity : m_entityManager.getEntities("Player"))
        {
            auto& trans = entity->getComponent<CTransform>();
			Vec2 pos = getGridPosition(trans.pos.x, trans.pos.y);
			auto& bb = entity->getComponent<CBoundingBox>();
			fout << entity->tag() << " " << pos.x << " " << pos.y << " " << bb.size.x << " " << bb.size.y << " " << m_playerConfig.JUMP << " " << m_playerConfig.SPEED << " "
				<< m_playerConfig.MAXSPEED << " " << m_playerConfig.GRAVITY << " " << m_playerConfig.HEALTH << "\n";
        }

		for (auto entity : m_entityManager.getEntities("Dec"))
		{
            auto& trans = entity->getComponent<CTransform>();
			Vec2 pos = getGridPosition(trans.pos.x, trans.pos.y);
			fout << "Dec" << ' ' << entity->getComponent<CAnimation>().animation.getName() << ' ' << pos.x << ' ' << pos.y << '\n';
		}

        for (auto entity : m_entityManager.getEntities())
        {
            auto& trans = entity->getComponent<CTransform>();

            if (entity->tag() == "Tile" || entity->tag() == "Exit")
            {
				Vec2 pos = getGridPosition(trans.pos.x, trans.pos.y);
				auto& bb = entity->getComponent<CBoundingBox>();
                if (entity->hasComponent<CPatrol>())
                {
                    auto& patrol = entity->getComponent<CPatrol>();
                    fout << "MTile" << ' ' << entity->getComponent<CAnimation>().animation.getName() << ' ' << pos.x << ' ' << pos.y << ' ' << bb.blockMove << ' ' << bb.blockVision << ' '
                        << patrol.speed << ' ' << patrol.positions.size();
                    for (size_t i = 0; i < patrol.positions.size(); i++)
                    {
						Vec2 patrolGridPos = getGridPosition(patrol.positions[i].x, patrol.positions[i].y);
						fout << ' ' << patrolGridPos.x << ' ' << patrolGridPos.y;
                    }
                    fout << '\n';
                }
                else
                {
                    fout << entity->tag() << " " << entity->getComponent<CAnimation>().animation.getName() << " " << pos.x << " " << pos.y << " " << bb.blockMove << " " << bb.blockVision << "\n";
                }
            }

            if (entity->tag() == "Mob")
            {
				Vec2 pos = getGridPosition(trans.pos.x, trans.pos.y);
                auto& bb = entity->getComponent<CBoundingBox>();
                fout << entity->tag() << ' ' << entity->getComponent<CAnimation>().animation.getName() << ' ' << pos.x << ' ' << pos.y << ' '
                    << bb.blockMove << ' ' << bb.blockVision << ' ' << entity->hasComponent<CGravity>() << ' ' << entity->getComponent<CHealth>().max << ' ' << entity->getComponent<CDamage>().damage << ' ';
                
                if (entity->hasComponent<CShootPlayer>())
                {
                    auto& shoot = entity->getComponent<CShootPlayer>();
                    auto& patrol = entity->getComponent<CPatrol>();
                    fout << "Boss" << ' ' << shoot.projectile << ' ' << shoot.attackSpeed << ' ' << shoot.attackRange << ' ' << patrol.speed << ' ' << patrol.positions.size();
                    for (size_t i = 0; i < patrol.positions.size(); i++)
                    {
						Vec2 patrolGridPos = getGridPosition(patrol.positions[i].x, patrol.positions[i].y);
						fout << ' ' << patrolGridPos.x << ' ' << patrolGridPos.y;
                    }
                    fout << '\n';
                }
                else if (entity->hasComponent<CFollowPlayer>())
                {
                    fout << "Follow" << ' ' << entity->getComponent<CFollowPlayer>().speed << ' ' << entity->getComponent<CFollowPlayer>().followDist << '\n';
                }
                else
                {
                    auto& patrol = entity->getComponent<CPatrol>();
                    fout << "Patrol" << ' ' << patrol.speed << ' ' << patrol.positions.size();
                    for (size_t i = 0; i < patrol.positions.size(); i++)
                    {
						Vec2 patrolGridPos = getGridPosition(patrol.positions[i].x, patrol.positions[i].y);
						fout << ' ' << patrolGridPos.x << ' ' << patrolGridPos.y;
                    }
                    fout << '\n';
                }
            }

            if (entity->tag() == "Item")
            {
				Vec2 pos = getGridPosition(trans.pos.x, trans.pos.y);
                fout << "Item" << ' ' << entity->getComponent<CAnimation>().animation.getName() << ' ' << pos.x << ' ' << pos.y << '\n';
            }
        }
    }

    std::cout << "Saved to temp.txt" << std::endl;
    fout.close();
}

void Scene_Editor::onEnd()
{
    m_game->playMusic("MainTheme");
	m_game->changeScene("WORLD", std::make_shared<Scene_World>(m_game, "world_view.txt"), true);
}

void Scene_Editor::openSettings()
{
    m_game->changeScene("SETTINGS", std::make_shared<Scene_Settings>(m_game, "EDIT"));
}

void Scene_Editor::update()
{
    if (!m_paused)
    {
        m_entityManager.update();
        sMovement();
        sCollision();
        sCamera();
        sAnimation();
        sAutoSave();
        m_currentFrame++;
    }
}

void Scene_Editor::spawnPlayer()
{
    m_player = m_entityManager.addEntity("Player");
    m_player->addComponent<CTransform>(getPosition(m_playerConfig.X, m_playerConfig.Y));
    m_player->addComponent<CAnimation>(m_game->assets().getAnimation("Stand"), true);
    m_player->addComponent<CBoundingBox>(Vec2(m_playerConfig.CX, m_playerConfig.CY), true, false);
    m_player->addComponent<CInput>();
    m_player->addComponent<CGravity>(m_playerConfig.GRAVITY);
    m_player->addComponent<CHealth>(m_playerConfig.HEALTH, m_playerConfig.HEALTH);
    m_player->addComponent<CDrag>();
}

void Scene_Editor::handleClick()
{
    if (CDrag::attached) {
        auto& trans = CDrag::attached->getComponent<CTransform>();
        Vec2 grid = getGridPosition(m_mousePos.x, m_mousePos.y);

        grid = grid.round();
        trans.pos = getPosition(grid.x, grid.y);

        CDrag::attached->removeComponent<CDrag>();
        CDrag::attached = nullptr;
    }
    else
    {
        for (auto entity : m_entityManager.getEntities())
        {
            if (Physics::IsInside(m_mousePos, entity))
            {
                entity->addComponent<CDrag>();
                CDrag::attached = entity;
                break;
            }
        }
    }
}

void Scene_Editor::insert(std::stringstream& args)
{
    std::string type;
	args >> type;
	if (type == "Player")
	{
		args >> m_playerConfig.X >> m_playerConfig.Y >> m_playerConfig.CX >> m_playerConfig.CY
			>> m_playerConfig.JUMP >> m_playerConfig.SPEED >> m_playerConfig.MAXSPEED >> m_playerConfig.GRAVITY >> m_playerConfig.HEALTH;
        spawnPlayer();
	}

	if (type == "Tile" || type == "Exit" || type == "MTile")
	{
		std::string anim;
		int gx, gy, bm, bv;
		args >> anim >> gx >> gy >> bm >> bv;
		auto tile = m_entityManager.addEntity(type == "Exit" ? type : "Tile");
		tile->addComponent<CAnimation>(m_game->assets().getAnimation(anim), true);
		tile->addComponent<CTransform>(getPosition(gx, gy));
		tile->addComponent<CBoundingBox>(m_game->assets().getAnimation(anim).getSize(), bm, bv);
		tile->addComponent<CDrag>();

        if (type == "MTile")
        {
			float s;
			int n;
			int x, y;
			std::vector<Vec2> points;
			args >> s >> n;
			for (int i = 0; i < n; i++)
			{
				args >> x >> y;
				points.push_back(getPosition(x, y));
			}
			tile->addComponent<CPatrol>(points, s);
        }
	}

	if (type == "Mob")
	{
		std::string anim, ai;
		float gx, gy, bm, bv, h, d, g;
		args >> anim >> gx >> gy >> bm >> bv >> g >> h >> d >> ai;
		auto npc = m_entityManager.addEntity("Mob");
		npc->addComponent<CAnimation>(m_game->assets().getAnimation(anim), true);
		npc->addComponent<CTransform>(getPosition(gx, gy));
		npc->addComponent<CBoundingBox>(m_game->assets().getAnimation(anim).getSize(), bm, bv);
		npc->addComponent<CDamage>(d);
		npc->addComponent<CHealth>(h, h);
        if (g)
        {
            npc->addComponent<CGravity>(1);
        }

		if (ai == "Boss")
		{
            std::string pr;
			float as, ar;
			args >> pr >> as >> ar;
			npc->addComponent<CShootPlayer>(pr, as, ar);
		}

		if (ai == "Follow")
		{
			float s, d;
			args >> s >> d;
			npc->addComponent<CFollowPlayer>(getPosition(gx, gy), s, d);
		}
		else if (ai == "Patrol" || ai == "Boss")
		{
			float s;
			int n;
			int x, y;
			std::vector<Vec2> points;
			args >> s >> n;
			for (int i = 0; i < n; i++)
			{
				args >> x >> y;
				points.push_back(getPosition(x, y));
			}

			npc->addComponent<CPatrol>(points, s);
		}
	}

    if (type == "Item")
    {
		std::string anim;
		float gx, gy;
		args >> anim >> gx >> gy;
		auto item = m_entityManager.addEntity("Item");
		item->addComponent<CTransform>(getPosition(gx, gy));
		item->addComponent<CBoundingBox>(m_game->assets().getAnimation(anim).getSize(), 0, 0);
		item->addComponent<CAnimation>(m_game->assets().getAnimation(anim), true);
    }

    if (type == "Dec")
    {
		std::string anim;
		float gx, gy;
		args >> anim >> gx >> gy;
		auto item = m_entityManager.addEntity("Dec");
		item->addComponent<CTransform>(getPosition(gx, gy));
		item->addComponent<CBoundingBox>(m_game->assets().getAnimation(anim).getSize(), 0, 0);
		item->addComponent<CAnimation>(m_game->assets().getAnimation(anim), true);
    }


    if (type == "Nav")
    {
        std::string map;
        args >> map;
        loadNavMap(map);
    }
}

void Scene_Editor::handleCopyClick()
{
	for (auto entity : m_entityManager.getEntities())
	{
		if (Physics::IsInside(m_mousePos, entity))
		{
            copyEntity(entity);
            break;
		}
	}
}

void Scene_Editor::copyEntity(std::shared_ptr<Entity> entity)
{
    std::stringstream ss;

    std::string tag = entity->tag();
    if (tag == "Player")
    {
        return;
    }
    else if (tag == "Tile" || tag == "Exit")
    {
        auto pos = entity->getComponent<CTransform>().pos;
        auto gridPos = getGridPosition(pos.x, pos.y).round();
        ss << tag << ' ' << entity->getComponent<CAnimation>().animation.getName() << ' ' << gridPos.x << ' ' << gridPos.y << ' '
            << entity->getComponent<CBoundingBox>().blockMove << ' ' << entity->getComponent<CBoundingBox>().blockVision;
    }

	if (tag == "Mob")
	{
        Vec2 pos = entity->getComponent<CTransform>().pos;
		Vec2 gridPos = getGridPosition(pos.x, pos.y);
		auto& bb = entity->getComponent<CBoundingBox>();
		 ss << entity->tag() << ' ' << entity->getComponent<CAnimation>().animation.getName() << ' ' << gridPos.x << ' ' << gridPos.y << ' '
			<< bb.blockMove << ' ' << bb.blockVision << ' ' << entity->hasComponent<CGravity>() << ' ' << entity->getComponent<CHealth>().max << ' ' << entity->getComponent<CDamage>().damage << ' ';
		if (entity->hasComponent<CFollowPlayer>())
		{
			ss << "Follow" << ' ' << entity->getComponent<CFollowPlayer>().speed << ' ' << entity->getComponent<CFollowPlayer>().followDist;
		}
		else
		{
			auto& patrol = entity->getComponent<CPatrol>();
			ss << "Patrol" << ' ' << patrol.speed << ' ' << patrol.positions.size();
			for (size_t i = 0; i < patrol.positions.size(); i++)
			{
                Vec2 patrolGridPos = getGridPosition(patrol.positions[i].x, patrol.positions[i].y);
				ss << ' ' << patrolGridPos.x << ' ' << patrolGridPos.y;
			}
		}
        std::cout << "copied " << ss.str() << std::endl;
	}

    if (tag == "Item" || tag == "Dec")
    {
        Vec2 pos = entity->getComponent<CTransform>().pos;
		Vec2 gridPos = getGridPosition(pos.x, pos.y);
        ss << tag << ' ' << entity->getComponent<CAnimation>().animation.getName() << ' ' << gridPos.x << ' ' << gridPos.y;
    }


    insert(ss);
}

void Scene_Editor::handleDestroyClick()
{
	for (auto entity : m_entityManager.getEntities())
	{
		if (Physics::IsInside(m_mousePos, entity))
		{
            entity->destroy();
            break;
		}
	}
}

void Scene_Editor::updateMousePos(const Vec2& pos)
{
    sf::View view = m_game->window().getView();
    auto center = view.getCenter();
    m_mousePos = Vec2(center.x - (m_game->window().getSize().x / 2) + pos.x, pos.y);
}

void Scene_Editor::sCollision()
{
    for (auto tile : m_entityManager.getEntities("Tile"))
    {
        for (auto actor : m_entityManager.getEntities("Player"))
        {
            Vec2 overlap = Physics::GetOverlap(actor, tile);
            if (overlap.x > 0 && overlap.y > 0)
            {
                Vec2 prevOverlap = Physics::GetPreviousOverlap(actor, tile);
                if (prevOverlap.x > 0 || (prevOverlap.x > 0 && prevOverlap.y > 0))
                {
                    bool above = actor->getComponent<CTransform>().pos.y < tile->getComponent<CTransform>().pos.y;
                    actor->getComponent<CTransform>().pos.y -= above ? overlap.y : -overlap.y;
                }
                else if (prevOverlap.y > 0)
                {
                    actor->getComponent<CTransform>().pos.x -= actor->getComponent<CTransform>().pos.x < tile->getComponent<CTransform>().pos.x ? overlap.x : -overlap.x;
                }
            }
        }
    }
}

void Scene_Editor::sMovement()
{
    if (CDrag::attached) 
    {
        CDrag::attached->getComponent<CTransform>().pos = m_mousePos;
    }
}

void Scene_Editor::sAutoSave()
{
    if (m_currentFrame % (60 * 60 * 3) == (60 * 60 * 3 - 1))
    {
        std::cout << "Autosaving...\n";
        saveToFile();
    }
}

void Scene_Editor::sAnimation() {}

void Scene_Editor::sCamera()
{
    sf::View view = m_game->window().getView();
    auto center = view.getCenter();
    if (m_player->getComponent<CInput>().left)
    {
        center.x -= 5;
    }
    if (m_player->getComponent<CInput>().right)
    {
        center.x += 5;
    }

    view.setCenter(fmax(center.x, m_game->window().getSize().x / 2), view.getCenter().y);
    m_game->window().setView(view);
}

void Scene_Editor::sRender()
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

    m_game->window().display();
}

void Scene_Editor::sDoAction(const Action& action)
{
    if (action.type() == "START")
    {
        if (action.name() == "TOGGLE_TEXTURE") { m_drawTextures = !m_drawTextures; }
        else if (action.name() == "TOGGLE_COLLISION") { m_drawCollision = !m_drawCollision; }
        else if (action.name() == "TOGGLE_GRID") { m_drawGrid = !m_drawGrid; }
        else if (action.name() == "TOGGLE_NAV") { m_drawNav = !m_drawNav; }
        else if (action.name() == "PAUSE") { setPaused(!m_paused); }
        else if (action.name() == "QUIT") { onEnd(); }

        else if (action.name() == "LEFT") { m_player->getComponent<CInput>().left = true; }
        else if (action.name() == "RIGHT") { m_player->getComponent<CInput>().right = true; }
        else if (action.name() == "UP") { m_player->getComponent<CInput>().up = true; }
        else if (action.name() == "DOWN") { m_player->getComponent<CInput>().down = true; }

        else if (action.name() == "LEFT_CLICK") { handleClick(); }
        else if (action.name() == "RIGHT_CLICK") { handleDestroyClick(); }
        else if (action.name() == "MIDDLE_CLICK") { handleCopyClick(); }

        else if (action.name() == "INSERT") 
        {
            std::cout << "Input New Entity:" << std::endl;
            std::string input;
            std::getline(std::cin, input);
            std::stringstream ss(input);
            insert(ss);
        }
        else if (action.name() == "SAVE_TO_FILE") { saveToFile(); }
    }
    else if (action.type() == "END")
    {
        if (action.name() == "LEFT") { m_player->getComponent<CInput>().left = false; }
        else if (action.name() == "RIGHT") { m_player->getComponent<CInput>().right = false; }
        else if (action.name() == "UP") { m_player->getComponent<CInput>().up = false; }
        else if (action.name() == "DOWN") { m_player->getComponent<CInput>().down = false; }
    }

    if (action.name() == "MOUSE_MOVE") { updateMousePos(action.pos()); }
}
