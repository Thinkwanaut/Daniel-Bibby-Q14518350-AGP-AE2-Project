#include "Enemy.h"

Enemy::Enemy(ID3D11Device* device, ID3D11DeviceContext* context, AssetManager* assets, char* model, char* texture, char* shader)
	: GameObject{ device, context, assets, model, texture, shader }
{
	SetScale(m_Size, m_Size, m_Size);
	SetCollisionType(ColliderShape::Box);

	mp_Timer = new Timer();
}

void Enemy::SetKey(char* model, char* texture)
{
	m_KeyModel = model;
	m_KeyTexture = texture;
}

void Enemy::Move(GameObject* target, std::vector<GameObject*> Obstacles, float lagAdjust)
{
	LookAt(target->GetX(), target->GetY(), target->GetZ());
	MoveForward(m_Speed, Obstacles, lagAdjust);

	if (XMVector4Equal(m_AddedColour, m_FlashCol) && mp_Timer->GetTimer("Flash") > m_FlashTime) m_AddedColour = { 0, 0, 0, 0 };
}

void Enemy::GetHit()
{
	m_Health--;
	m_AddedColour = m_FlashCol;
	mp_Timer->StartTimer("Flash");
}

bool Enemy::IsDead()
{
	return m_Health <= 0;
}

GameObject* Enemy::SpawnKey()
{
	GameObject* newKey = new GameObject(mp_D3DDevice, mp_ImmediateContext, mp_Assets, m_KeyModel, m_KeyTexture, (char*)"shaders.hlsl");
	newKey->SetCollisionType(ColliderShape::Box);
	newKey->SetPos(GetPos());
	newKey->SetScale(3.0f, 3.0f, 3.0f);
	newKey->SetRot(0.0f, m_yRot, m_zRot);
	newKey->SetColour({ 1, 1, 0, 1 });
	return newKey;
}
