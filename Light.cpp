#include "Light.h"

Light::Light(XMVECTOR colour)
	: m_colour{ colour }
{
}

void Light::SetColour(XMVECTOR colour)
{
	m_colour = colour;
}

void Light::AddColour(XMVECTOR colour, bool clamp)
{
	m_colour += colour;
	if (clamp) m_colour = XMVectorClamp(m_colour, { 0, 0, 0, 0 }, { 1, 1, 1, 1 });
}

XMVECTOR Light::Colour()
{
	return m_colour;
}
