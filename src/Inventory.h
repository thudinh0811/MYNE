#pragma once

#include <vector>
#include <string>
#include "Components.h"
#include "EntityManager.h"
#include "GameEngine.h"

typedef void (*useFunc) (std::shared_ptr<Entity>, std::shared_ptr<Entity>, EntityManager&, GameEngine* game);

class Inventory
{
private:
	EntityManager m_items;
	GameEngine* m_game = nullptr;
	int m_index = 0;
	void makeItem(const std::string& tag);
	void removeItem(const std::string& tag);
	void static swing(std::shared_ptr<Entity> user, std::shared_ptr<Entity> item, EntityManager& manager, GameEngine* game);
	void static shoot(std::shared_ptr<Entity> user, std::shared_ptr<Entity> item, EntityManager& manager, GameEngine* game);
	void static drink(std::shared_ptr<Entity> user, std::shared_ptr<Entity> item, EntityManager& manager, GameEngine* game);
public:
	Inventory();
	std::shared_ptr<Entity> getItem();
	std::shared_ptr<Entity> getNextItem();
	std::shared_ptr<Entity> getLastItem();
	std::shared_ptr<Entity> peekNextItem();
	std::shared_ptr<Entity> peekLastItem();
	void addItem(const std::string& tag);
	useFunc useItem(const std::string& tag);
	void setEngineReference(GameEngine* engine);
	bool hasItems();
};