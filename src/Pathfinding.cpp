#include "Pathfinding.h"
#include "Scene_Play.h"
#include "Scene.h"

Pathfinding::Node Pathfinding::bfs(Vec2 target, const NavMap& map, std::vector<Vec2>& closed, std::vector<Node>& open, Scene_Play* scene)
{
	if (open.size() == 0)
	{
		Node n;
		return n;
	}
	auto node = open[0];
	open.erase(open.begin());
	closed.push_back(node.current->position);

	auto np = scene->getPosition(node.current->position.x, node.current->position.y);
	if (scene->canSee(target, np) && target.y - np.y < 80)
	{
		return node;
	}
	else
	{
		for (auto next : node.current->connections)
		{
			Node newNode(map[next], *(node.current));
			if (std::find(closed.begin(), closed.end(), map[next].position) == closed.end()
				&& std::find(open.begin(), open.end(), newNode) == open.end())
			{
				open.push_back(newNode);
			}
		}
	}

	return bfs(target, map, closed, open, scene);
}

std::vector<Vec2> Pathfinding::findPath(Vec2 start, Vec2 target, const NavMap& map, Scene_Play* scene)
{
	std::vector<Vec2> path;
	Vec2 targetPos = scene->getGridPosition(target.x, target.y);

	size_t closest = 0;
	float minDist = 1.0e32f;
	for (size_t i = 0; i < map.size(); i++)
	{
		auto mp = scene->getPosition(map[i].position.x, map[i].position.y);
		float dist = target.dist(mp);
		if (dist < minDist && scene->canSee(target, mp))
		{
			closest = i;
			minDist = dist;
		}
	}

	if (start.dist(target) < minDist && scene->canSee(target, start))
	{
		path.push_back(targetPos);
		return path;
	}

	std::vector<Vec2> closed;
	std::vector<Node> open;

	open.push_back(Node(map[closest]));
	Node goal = bfs(start, map, closed, open, scene);
	if (goal.current != nullptr)
	{
		path.push_back(goal.current->position);
		if (goal.parent != nullptr)
		{
			path.push_back(goal.parent->position);
		}
		else
		{
			path.push_back(targetPos);
		}
	}
	return path;
}

bool Pathfinding::Node::operator==(const Node& rhs)
{
	return current->position == rhs.current->position;
}
