#include "Bullet.h"

Bullet::Bullet(ID3D11Device* device, ID3D11DeviceContext* context, AssetManager* assets, char* model, char* texture, char* shader)
	: GameObject{ device, context, assets, model, texture, shader }
{
	SetScale(m_Size, m_Size, m_Size);
	SetCollisionType(ColliderShape::Box);
	SetColour({ 0, 1.0f, 1.0f, 1.0f });
}

void Bullet::Shoot(XMFLOAT3 start, XMFLOAT3 target, float speed, int damage, float range, bool throughEnemies)
{
	m_Start = start;
	SetPos(start.x, start.y, start.z);
	XMVECTOR targetVec = XMVectorSet(target.x - start.x, target.y - start.y, target.z - start.z, 0.0f);
	m_Direction = XMVector3Normalize(targetVec);
	LookAtRelative(XMVectorGetX(m_Direction), XMVectorGetY(m_Direction), XMVectorGetZ(m_Direction));
	m_Speed = speed;
	m_MaxDSq = powf(range, 2.0f);
	m_Damage = damage;
	m_ThroughEnemies = throughEnemies;
}

bool Bullet::Move(std::vector<GameObject*> Obstacles, float lagAdjust)
{
	MoveForward(m_Speed, lagAdjust);

	for (GameObject* o : Obstacles)
		if (CheckCollision(o)) return true;

	XMVECTOR aToB = XMVectorSet(m_x - m_Start.x, m_y - m_Start.y, m_z - m_Start.z, 0.0f); // Check for max travel distance, then destroy
	float d_sq = powf(XMVectorGetX(aToB), 2) + powf(XMVectorGetY(aToB), 2) + powf(XMVectorGetZ(aToB), 2);

	return d_sq > m_MaxDSq;
}

bool Bullet::TargetCheck(std::vector<Enemy*> Enemies, int* index)
{
	*index = -1;
	for (int e = 0; e < Enemies.size(); e++)
	{
		if (CheckCollision(Enemies[e]) || PassedThrough(Enemies[e]))
		{
			*index = e;
			Enemies[e]->GetHit(m_Damage);
		}
	}
	return *index != -1 && !m_ThroughEnemies;
}
