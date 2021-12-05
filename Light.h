#pragma once
#include <d3d11.h>
#include <math.h>

#define _XM_NO_INSTRINSICS_
#define XM_NO_ALIGNMENT
#include <DirectXMath.h>

using namespace DirectX;
class Light
{
protected:
	XMVECTOR m_colour;

public:
	Light(XMVECTOR colour);

	XMVECTOR Colour();

	void SetColour(XMVECTOR colour);
	void AddColour(XMVECTOR colour, bool clamp = false);
};

