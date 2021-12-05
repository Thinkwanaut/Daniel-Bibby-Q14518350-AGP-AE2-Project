#include "Enemy.h"

Enemy::Enemy(ID3D11Device* device, ID3D11DeviceContext* context, AssetManager* assets, char* model, char* texture, char* shader)
	: GameObject{ device, context, assets, model, texture, shader }
{
	SetCollisionType(ColliderShape::Box);
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
}

void Enemy::GetHit()
{
	m_Health--;
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
	newKey->SetScale(0.15f, 0.15f, 0.15f);
	newKey->SetRot(0.0f, m_yRot, m_zRot);
	newKey->SetColour({ 1, 1, 0, 1 });
	return newKey;
}
