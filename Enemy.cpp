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

	if (mp_Timer->GetTimer("Flash") > m_FlashTime) m_AddedColour = { 0, 0, 0, 0 };
}

void Enemy::GetHit(int damage)
{
	m_Health -= damage;
	m_AddedColour = m_FlashCol;
	mp_Timer->StartTimer("Flash");
}

bool Enemy::IsDead()
{
	return m_Health <= 0;
}

GameObject* Enemy::SpawnKey(bool addParticles)
{
	GameObject* newKey = new GameObject(mp_D3DDevice, mp_ImmediateContext, mp_Assets, m_KeyModel, m_KeyTexture, (char*)"shaders.hlsl");
	newKey->SetCollisionType(ColliderShape::Box);
	newKey->SetPos(GetPos());
	newKey->SetScale(m_KeySize, m_KeySize, m_KeySize);
	newKey->SetRot(0.0f, m_yRot, m_zRot);
	newKey->SetTint({ 1.0f, 0.0f, 0.0f, 1.0f });
	newKey->AddTintCycle({ 1.0f, 1.0f, 0.0f, 1.0f });
	newKey->AddTintCycle({ 0.0f, 1.0f, 0.0f, 1.0f });
	newKey->AddTintCycle({ 0.0f, 1.0f, 1.0f, 1.0f });
	newKey->AddTintCycle({ 0.0f, 0.0f, 1.0f, 1.0f });
	newKey->AddTintCycle({ 1.0f, 0.0f, 1.0f, 1.0f });
	newKey->SetCycleTime(0.5f);

	if (addParticles) newKey->MakeParticles(Presets::KEY);

	return newKey;
}
