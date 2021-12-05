#pragma once
#include "Light.h"

class DirectionalLight : public Light
{
private:
	XMVECTOR m_direction;
	XMFLOAT3 m_rotation{ 0, 0, 0 };

public:
	DirectionalLight(XMVECTOR direction, XMVECTOR colour);

	XMVECTOR Direction();

	void SetDirection(XMVECTOR direction);
	void SetRotation(XMFLOAT3 rotation);
	void Rotate(XMFLOAT3 rotation);
};

