#include "Bullet.h"

Bullet::Bullet(ID3D11Device* device, ID3D11DeviceContext* context, AssetManager* assets, char* model, char* texture, char* shader)
	: GameObject{ device, context, assets, model, texture, shader }
{
	SetScale(m_Size, m_Size, m_Size);
	SetCollisionType(ColliderShape::Box);
	SetColour({ 0, 1, 1, 0 });
}

void Bullet::Shoot(XMVECTOR start, XMVECTOR target)
{
	m_Start = start;
	SetPos(XMVectorGetX(start), XMVectorGetY(start), XMVectorGetZ(start));
	XMVECTOR targetVec = target - start;
	m_Direction = XMVector3Normalize(target - start);
}

bool Bullet::Move(std::vector<GameObject*> Obstacles, float lagAdjust)
{
	m_x += XMVectorGetX(m_Direction) * m_Speed * lagAdjust;
	m_y += XMVectorGetY(m_Direction) * m_Speed * lagAdjust;
	m_z += XMVectorGetZ(m_Direction) * m_Speed * lagAdjust;

	for (GameObject* o : Obstacles)
		if (CheckCollision(o)) return true;

	XMVECTOR aToB = XMVectorSet(m_x, m_y, m_z, 0.0f) - m_Start; // Check for max travel distance, then destroy
	float d_sq = powf(XMVectorGetX(aToB), 2) + powf(XMVectorGetY(aToB), 2) + powf(XMVectorGetZ(aToB), 2);

	return d_sq > m_MaxDSq;
}

int Bullet::TargetCheck(std::vector<Enemy*> Enemies)
{
	for (int e = 0; e < Enemies.size(); e++)
		if (CheckCollision(Enemies[e]) || PassedThrough(Enemies[e])) return e;

	return -1;
}
