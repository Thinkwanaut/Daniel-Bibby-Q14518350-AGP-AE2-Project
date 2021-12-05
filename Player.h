#pragma once
#include <d3d11.h>
#include <math.h>

#define _XM_NO_INSTRINSICS_
#define XM_NO_ALIGNMENT
#include <DirectXMath.h>

#include "GameObject.h"
#include "Bullet.h"
#include "Input.h"

using namespace DirectX;
enum class Collectable
{
	HEALTH, KEY
};

class Player : public GameObject
{
private:
	float m_dx{ 0 }, m_dy{ 0 }, m_dz{ 0 }, m_CamRotX{ 0 }, m_CamRotY{ 0 }, m_AngleClamp{ 89.9f }, m_LookX{ 0 }, m_LookZ{ 0 };
	float m_speed{ 0.05f }, m_JumpSpeed{ .25f }, m_Sprint{ 3 };
	XMVECTOR m_position{}, m_lookAt{}, m_up{};
	XMVECTOR m_GunOffset{ 0.5, 0.5, 0.5 };
	float m_TargetDist{ 20 }, m_MaxHealth{ 10 }, m_Health{ 10 };
	float m_ThrowVelX{ 0 }, m_ThrowVelZ{ 0 };

	bool m_Thrown{ false };

	int m_Score{ 0 };

	char* m_BulletModel = nullptr;
	char* m_BulletTexture = nullptr;
	
public:
	Player(ID3D11Device* device, ID3D11DeviceContext* context, AssetManager* assets, char* model, char* texture, char* shader);
	void Move(Input* input, std::vector<GameObject*> obstacles, float grav, float adjust = 1);
	void Forward(float movement, std::vector<GameObject*> obstacles);
	void Strafe(float movement, std::vector<GameObject*> obstacles);
	void Fly(float zAdd);
	void Jump(bool jumpAnyway = false);
	void SetXRot(float xRot);
	void SetYRot(float yRot);
	void AddXRot(float xAdd);
	void AddYRot(float yAdd);
	void SetCameraLook();
	void SetBullet(char* model, char* texture);

	Bullet* Shoot();

	void SpikeCheck(std::vector<GameObject*> spikes);
	int EnemyCheck(std::vector<Enemy*> enemies);

	int CollectItem(std::vector<GameObject*> items, Collectable itemType);
	int Health();
	int Score();

	XMMATRIX GetViewMatrix();
	XMFLOAT3 GetPos();
	XMFLOAT2 Normalise2D(XMFLOAT2 vector);
};

