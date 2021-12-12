#include "MoreMaths.h"

XMFLOAT2 Normalise2D(XMFLOAT2 vector2)
{
	float magnitude = powf(powf(vector2.x, 2.0f) + powf(vector2.y, 2), 0.5f);
	if (magnitude > 0)
		return XMFLOAT2{ vector2.x / magnitude, vector2.y / magnitude };
	else return vector2;
}

XMFLOAT3 Normalise3D(XMFLOAT3 vector3)
{
	float magnitude = powf(powf(vector3.x, 2.0f) + powf(vector3.y, 2.0f) + powf(vector3.z, 2.0f), 0.5f);
	if (magnitude > 0)
		return XMFLOAT3{ vector3.x / magnitude, vector3.y / magnitude, vector3.z / magnitude };
	else return vector3;
}

XMFLOAT3 CrossProduct(XMFLOAT3 a, XMFLOAT3 b)
{
	XMFLOAT3 c = { 0.0f, 0.0f, 0.0f };

	c.x = (a.y * b.z - a.z * b.y);
	c.y = (a.z * b.x - a.x * b.z);
	c.z = (a.x * b.y - a.y * b.x);

	return c;
}

float Lerp(float a, float b, float t)
{
	return a + (b - a) * t;
}

XMFLOAT2 Lerp2(XMFLOAT2 a, XMFLOAT2 b, float t)
{
	return { Lerp(a.x, b.x, t), Lerp(a.y, b.y, t) };
}

XMFLOAT3 Lerp3(XMFLOAT3 a, XMFLOAT3 b, float t)
{
	return { Lerp(a.x, b.x, t), Lerp(a.y, b.y, t), Lerp(a.z, b.z, t) };
}

XMFLOAT4 Lerp4(XMFLOAT4 a, XMFLOAT4 b, float t)
{
	return { Lerp(a.x, b.x, t), Lerp(a.y, b.y, t), Lerp(a.z, b.z, t), Lerp(a.w, b.w, t) };
}

float Random01()
{
	return (float)rand() / (float)RAND_MAX;
}

float RandomRange(float min, float max)
{
	return Lerp(min, max, Random01());
}

float RandomPN(float scale)
{
	return (Random01() - 0.5f) * 2.0f * scale;
}

float RandomMultiplier(float scale)
{
	return 1.0 + RandomPN(scale);
}

XMFLOAT2 Random2(float minscale, float maxscale)
{
	XMFLOAT2 baseVec = Normalise2D({ RandomPN(), RandomPN() });
	float scale = RandomRange(minscale, maxscale);

	return { baseVec.x * scale, baseVec.y * scale };
}

XMFLOAT3 Random3(float minscale, float maxscale)
{
	XMFLOAT3 baseVec = Normalise3D({ RandomPN(), RandomPN(), RandomPN() });
	float scale = RandomRange(minscale, maxscale);

	return { baseVec.x * scale, baseVec.y * scale, baseVec.z * scale };
}
