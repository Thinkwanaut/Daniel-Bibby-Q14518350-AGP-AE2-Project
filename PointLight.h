#pragma once
#include "Light.h"

class PointLight : public Light
{
private:
	XMVECTOR m_position;

public:
	PointLight(XMVECTOR position, XMVECTOR colour);

	XMVECTOR Position();

	void SetPosition(XMVECTOR position);
	void Move(XMVECTOR movement);
};