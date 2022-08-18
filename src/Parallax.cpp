#include "Parallax.h"

Parallax::Parallax(Animation* anims, float* strength, Vec2 startpos)
{
	for (int i = 0; i < layer_count; i++)
	{
		m_lengths[i] = anims[i].getSize().x;
		m_currentpos[i] = startpos.x;
		for (int j = 0; j < layer_parts; j++)
		{
			m_layers[i][j] = anims[i];
			m_startpos[i][j] = startpos.x + ((j-1) * m_lengths[i]);
			m_layers[i][j].getSprite().setOrigin(0, anims[i].getSize().y);
			m_layers[i][j].getSprite().setPosition(m_startpos[i][j], startpos.y);
		}
		
		m_strenths[i] = strength[i];
	}
}

Parallax Parallax::operator=(const Parallax& other)
{
	for (int i = 0; i < layer_count; i++)
	{
		m_lengths[i] = other.m_lengths[i];
		for (int j = 0; j < layer_parts; j++)
		{
			m_layers[i][j] = other.m_layers[i][j];
			m_startpos[i][j] = other.m_startpos[i][j];
		}
		m_strenths[i] = other.m_strenths[i];
	}
	return *this;
}

void Parallax::update(float dx)
{
	for (int i = 0; i < layer_count; i++)
	{
		float dist = dx * (1 - m_strenths[i]);
		float displ = dx * m_strenths[i];
		for (int j = 0; j < layer_parts; j++)
		{
			m_layers[i][j].getSprite().setPosition(m_startpos[i][j] + m_currentpos[i] + displ, m_layers[i][j].getSprite().getPosition().y);
		}

		if (dist > (m_currentpos[i] + m_lengths[i])) m_currentpos[i] += m_lengths[i];
		else if (dist < (m_currentpos[i] - m_lengths[i])) m_currentpos[i] -= m_lengths[i];
	}
}

void Parallax::drawBack(sf::RenderWindow& window)
{
	for (int i = 0; i < layer_count - 1; i++)
	{
		for (int j = 0; j < layer_parts; j++)
		{
			window.draw(m_layers[i][j].getSprite());
		}
	}
}

void Parallax::drawFront(sf::RenderWindow& window)
{
		for (int j = 0; j < layer_parts; j++)
		{
			window.draw(m_layers[layer_count-1][j].getSprite());
		}
}