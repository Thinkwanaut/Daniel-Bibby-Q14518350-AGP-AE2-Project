#include "PointLight.h"

PointLight::PointLight(XMVECTOR position, XMVECTOR colour)
	: m_position{ position }, Light{ colour }
{
}

XMVECTOR PointLight::Position()
{
	return m_position;
}

void PointLight::SetPosition(XMVECTOR position)
{
	m_position = position;
}

void PointLight::Move(XMVECTOR movement)
{
	m_position += movement;
}
