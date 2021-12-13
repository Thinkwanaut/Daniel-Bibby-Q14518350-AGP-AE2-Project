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

bool Bullet::Move(std::vector<GameObject*> Obstacles, std::vector<Enemy*> enemies, int* index, float adjust, bool sweepDetection)
{
	m_Hit = false;
	float sweep = (sweepDetection && enemies.size() > 0) ? enemies[0]->GetScale().z : m_Speed * adjust;
	float moved = 0.0f;

	// Move bullets in smaller increments (no greater than enemy size) so that they do not pass over enemies in a single frame
	// More expensive to run (hence optional boolean) but ensures more consistent gameplay

	while (moved < m_Speed * adjust)
	{
		float nm = min(sweep, m_Speed * adjust - moved);

		float xm = sin(XMConvertToRadians(m_yRot)) * nm * cos(XMConvertToRadians(m_xRot));
		float ym = -sin(XMConvertToRadians(m_xRot)) * nm;
		float zm = cos(XMConvertToRadians(m_yRot)) * nm * cos(XMConvertToRadians(m_xRot));

		m_x += xm; m_y += ym; m_z += zm;

		if (TargetCheck(enemies, index)) return true;
		for (GameObject* o : Obstacles) 
			if (CheckCollision(o)) return true;

		moved += nm;
	}

	XMVECTOR aToB = XMVectorSet(m_x - m_Start.x, m_y - m_Start.y, m_z - m_Start.z, 0.0f); // Check for max travel distance, then destroy
	float d_sq = powf(XMVectorGetX(aToB), 2) + powf(XMVectorGetY(aToB), 2) + powf(XMVectorGetZ(aToB), 2);

	return d_sq > m_MaxDSq;
}

bool Bullet::TargetCheck(std::vector<Enemy*> Enemies, int* index)
{
	if (m_Hit) return false; // Avoid bullets hitting enemies multiple times per frame

	*index = -1;
	for (int e = 0; e < Enemies.size(); e++)
	{
		if (CheckCollision(Enemies[e]) || PassedThrough(Enemies[e]))
		{
			*index = e;
			Enemies[e]->GetHit(m_Damage);
			m_Hit = true;
			if (!m_ThroughEnemies) return true;
		}
	}

	return false;
}
