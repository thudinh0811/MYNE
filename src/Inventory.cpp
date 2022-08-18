#include "Inventory.h"
#include <cassert>

bool Inventory::hasItems()
{
	return m_items.getEntities().size() > 0;
}

void Inventory::makeItem(const std::string& tag)
{
	assert(m_game != nullptr);
	auto item = m_items.addEntity(tag);
	if (tag == "Gun")
	{
		item->addComponent<CAnimation>(m_game->assets().getAnimation("Gun"), true);
		item->addComponent<CDamage>(1);
		item->addComponent<CCount>(-1);
		item->addComponent<CItem>("range", 40);
	}
	if (tag == "Sword")
	{
		item->addComponent<CAnimation>(m_game->assets().getAnimation("Sword"), true);
		item->addComponent<CDamage>(2);
		item->addComponent<CCount>(-1);
		item->addComponent<CItem>("melee", 20);
	}
	if (tag == "Hammer")
	{
		item->addComponent<CAnimation>(m_game->assets().getAnimation("Hammer"), true);
		item->addComponent<CDamage>(3);
		item->addComponent<CCount>(-1);
		item->addComponent<CItem>("melee", 48);
	}
	if (tag == "Health")
	{
		item->addComponent<CAnimation>(m_game->assets().getAnimation("Health"), true);
		item->addComponent<CCount>(1);
		item->addComponent<CStatusEffect>("HP", 30);
		item->addComponent<CItem>("drink", 4);
	}
	if (tag == "Speed")
	{
		item->addComponent<CCount>(1);
		item->addComponent<CAnimation>(m_game->assets().getAnimation("Speed"), true);
		item->addComponent<CItem>("drink", 4);
		item->addComponent<CStatusEffect>("SP", 600);
	}
	if (tag == "Damage")
	{
		item->addComponent<CCount>(1);
		item->addComponent<CAnimation>(m_game->assets().getAnimation("Damage"), true);
		item->addComponent<CItem>("drink", 4);
		item->addComponent<CStatusEffect>("DMG", 600);
	}

	m_items.update();
}

void Inventory::removeItem(const std::string& tag)
{
	m_items.getEntities(tag)[0]->destroy();

	m_items.update();
	m_index = m_index % m_items.getEntities().size();
}

Inventory::Inventory()
{
}

std::shared_ptr<Entity> Inventory::getItem()
{
	if (m_items.getEntities().size() > 0)
	{
		return m_items.getEntities()[m_index];
	}
	else
	{
		return nullptr;
	}
}

std::shared_ptr<Entity> Inventory::getNextItem()
{
	auto& items = m_items.getEntities();
	m_index = (m_index + 1) % items.size();
	return getItem();
}

std::shared_ptr<Entity> Inventory::getLastItem()
{
	auto& items = m_items.getEntities();
	m_index = m_index == 0 ? items.size() : m_index;
	m_index = (m_index - 1) % items.size();
	return getItem();
}

std::shared_ptr<Entity> Inventory::peekNextItem()
{
	if (m_items.getEntities().size() > 0)
	{
		return m_items.getEntities()[(m_index + 1) % m_items.getEntities().size()];
	}
	else
	{
		return nullptr;
	}
}

std::shared_ptr<Entity> Inventory::peekLastItem()
{
	if (m_items.getEntities().size() > 0)
	{
		return m_items.getEntities()[(m_index - 1) >= 0 ? (m_index - 1) : (m_items.getEntities().size() - 1)];
	}
	else
	{
		return nullptr;
	}
}

void Inventory::addItem(const std::string& tag)
{
	if (m_items.getEntities(tag).size() == 0)
	{
		makeItem(tag);
	}
	else
	{
		m_items.getEntities(tag)[0]->getComponent<CCount>().count++;
	}
}

useFunc Inventory::useItem(const std::string& tag)
{
	if (m_items.getEntities(tag).size() > 0)
	{
		auto item = m_items.getEntities(tag)[0];
		item->getComponent<CCount>().count--;
		if (item->getComponent<CCount>().count == 0)
		{
			removeItem(tag);
		}
		
		if (item->getComponent<CItem>().type == "range")
		{
			return shoot;
		}
		else if (item->getComponent<CItem>().type == "drink")
		{
			return drink;
		}
		else
		{
			return swing;
		}
	}
	return [](std::shared_ptr<Entity> user, std::shared_ptr<Entity> item, EntityManager& manager, GameEngine* game) {};
}

void Inventory::setEngineReference(GameEngine* engine)
{
	m_game = engine;
}

void Inventory::swing(std::shared_ptr<Entity> user, std::shared_ptr<Entity> item, EntityManager& manager, GameEngine* game)
{
	if (item->hasComponent<CWeaponReset>()) { return; }
	item->getComponent<CTransform>().scale.y = -1;
	item->addComponent<CBoundingBox>(item->getComponent<CAnimation>().animation.getSize(), true, false);
	item->getComponent<CWeaponReset>().has = true;
	game->playSound("Swing");
}

void Inventory::shoot(std::shared_ptr<Entity> user, std::shared_ptr<Entity> item, EntityManager& manager, GameEngine* game)
{
	if (item->hasComponent<CWeaponReset>()) { return; }
	auto dir = user->getComponent<CTransform>().scale;
	auto bullet = manager.addEntity("bullet");
	bullet->addComponent<CAnimation>(game->assets().getAnimation("Bullet"), true);
	bullet->addComponent<CBoundingBox>(game->assets().getAnimation("Bullet").getSize(), false, false);
	bullet->addComponent<CTransform>(item->getComponent<CTransform>().pos);
	bullet->addComponent<CLifeSpan>(60);
	bullet->getComponent<CTransform>().velocity = Vec2(10, 0) * dir.x;
	bullet->getComponent<CTransform>().scale.x = dir.x;
	item->getComponent<CWeaponReset>().has = true;
	game->playSound("GunShot");
}

void Inventory::drink(std::shared_ptr<Entity> user, std::shared_ptr<Entity> item, EntityManager& manager, GameEngine* game)
{
	if (item->hasComponent<CWeaponReset>()) { return; }
	if (!item->hasComponent<CStatusEffect>()) { return; }
	user->addComponent<CStatusEffect>(item->getComponent<CStatusEffect>().effect, item->getComponent<CStatusEffect>().duration);
	item->getComponent<CWeaponReset>().has = true;
	game->playSound("Drink");
}
