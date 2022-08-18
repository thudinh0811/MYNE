#pragma once

#include "Animation.h"

class Parallax
{
private:
	const int layer_count = 4;
	const int layer_parts = 3;
	float m_lengths[4] = { 0 };
	float m_strenths[4] = { 0 };
	float m_currentpos[4] = { 0 };
	float m_startpos[4][3] = { 0 };

	Animation m_layers[4][3];

public:
	Parallax() = default;
	Parallax(Animation* anims, float* strength, Vec2 startpos);
	void update(float dx);
	void drawBack(sf::RenderWindow& window);
	void drawFront(sf::RenderWindow& window);
	Parallax operator=(const Parallax& other);
};