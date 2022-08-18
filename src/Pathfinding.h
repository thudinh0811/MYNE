#pragma once

#include "Scene.h"
#include "Scene_Play.h"
#include "Vec2.h"
#include "Physics.h"
#include "Components.h"

namespace Pathfinding
{
	struct Node 
	{
		const NavPoint* current = 0;
		const NavPoint* parent = 0;
		Node() {}
		Node(const NavPoint& current) : current(&current) {}
		Node(const NavPoint& current, const NavPoint& parent) : current(&current), parent(&parent) {}

		bool operator == (const Node& rhs);
	};

	std::vector<Vec2> findPath(Vec2 start, Vec2 target, const NavMap& map, Scene_Play* scene);
	Node bfs(Vec2 target, const NavMap& map, std::vector<Vec2>& closed, std::vector<Node>& open, Scene_Play* scene);
}
