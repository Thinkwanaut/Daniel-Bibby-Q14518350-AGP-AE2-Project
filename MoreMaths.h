#pragma once
#include <d3d11.h>
#include <d3dx11.h>
#include <dxerr.h>
#define _XM_NO_INSTRINSICS_
#define XM_NO_ALIGNMENT
#include <DirectXMath.h>

// MADE AS AN ALTERNATIVE TO HAVING TO USE VECTORS FOR EVERYTHING

using namespace DirectX;
XMFLOAT2 Normalise2D(XMFLOAT2 vector2);
XMFLOAT3 Normalise3D(XMFLOAT3 vector3);
XMFLOAT3 CrossProduct(XMFLOAT3 a, XMFLOAT3 b);

float Lerp(float a, float b, float t);
XMFLOAT2 Lerp2(XMFLOAT2 a, XMFLOAT2 b, float t);
XMFLOAT3 Lerp3(XMFLOAT3 a, XMFLOAT3 b, float t);
XMFLOAT4 Lerp4(XMFLOAT4 a, XMFLOAT4 b, float t);

float Random01();
float RandomPN(float scale = 1);
float RandomMultiplier(float scale);
float RandomRange(float minscale, float maxscale);

XMFLOAT2 Random2(float minscale, float maxscale);
XMFLOAT3 Random3(float minscale, float maxscale);