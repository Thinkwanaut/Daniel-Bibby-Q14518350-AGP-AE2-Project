#pragma once
#include "GameObject.h"
#include "Bullet.h"
#include "Input.h"
#include "Timer.h"
#include "text2D.h"
#include "MoreMaths.h"

enum class Collectable
{
	HEALTH, KEY
};

struct Gun
{
	float ShotInterval{ 0.05f }, HipAccuracy{ 0.9f }, ZoomAccuracy{ 0.95f }, BulletSpeed{ 1.0f }, Range{ 200.0f };
	int Damage{ 1 }, ShotNum{ 1 };
	bool ThroughEnemies{ false };
	string HipCrosshair{ (char*)"(  )" };
	string ZoomCrosshair{ (char*)"(+)" };
	float ZoomAngle{ 60 };
};

class Player : public GameObject
{
private:
	float m_dx{ 0 }, m_dy{ 0 }, m_dz{ 1.0f }, m_CamRotX{ 0 }, m_CamRotY{ 0 }, m_AngleClamp{ 89.9f }, m_LookX{ 0 }, m_LookZ{ 1.0f };
	float m_speed{ 0.05f }, m_JumpSpeed{ .25f }, m_Sprint{ 2.0 };
	XMVECTOR m_position{ 0.0f, 0.0f, 0.0f, 0.0f }, m_lookAt{ 0.0f, 0.0f, 0.0f, 0.0f }, m_up{ 0.0f, 1.0f, 0.0f, 0.0f };
	XMFLOAT3 m_HipOffset{ 2.0f, 3.0f, 3.0f }, m_ZoomOffset{ 0.5f, 1.5f, 3.0f };
	XMFLOAT4 m_DefaultTextColour{ 1.0f, 1.0f, 1.0f, 0.7f };
	XMFLOAT4 m_HealthColour{ 1.0f, 1.0f, 1.0f, 1.0f }, m_ScoreColour{ 1.0f, 1.0f, 1.0f, 1.0f }, m_SpikeColour{ 1.0f, 1.0f, 1.0f, 1.0f };
	float m_TargetDist{ 200 }, m_MaxHealth{ 10 }, m_Health{ 10 }, m_FlashTime{ 0.2f }, m_DefaultProjectionAngle{ 90.0f };
	float m_ThrowVelX{ 0 }, m_ThrowVelZ{ 0 }, m_MinPull{ 0.1f }, m_MaxPull{ 0.2f }, m_PullRangeSq{ 2000.0f };

	bool m_Thrown{ false }, m_Zoomed{ false };

	int m_Score{ 0 }, m_MaxScore{ 100 }, m_SpikeScore{ 0 }, m_MaxSpikes{ 10 };

	char* m_BulletModel = nullptr;
	char* m_BulletTexture = nullptr;

	std::vector<Gun> m_Guns;
	int m_GunIndex{ 0 };

	GameObject* mp_GunModel = nullptr;
	Text2D* mp_2DText = nullptr;
	Timer* mp_Timer;
	
public:
	Player(ID3D11Device* device, ID3D11DeviceContext* context, AssetManager* assets, char* model, char* texture, char* shader);
	~Player();
	void Move(Input* input, std::vector<GameObject*> obstacles, float floorHeight, float grav, float adjust = 1);
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

	bool ShotReady();
	float GetRandTarget();
	void SetGun(Input* input); 
	std::vector<Bullet*> Shoot(float gap = 0.1f);

	void SpikeCheck(std::vector<GameObject*> spikes);
	int EnemyCheck(std::vector<Enemy*> enemies);
	void GetHit();

	void Magnet(std::vector<GameObject*> items, float adjust = 1.0f);
	int CollectItem(std::vector<GameObject*> items, Collectable itemType);
	void SetSpikeScore(int spikes, int maxSpikes);
	bool Dead();
	bool Won();

	float GetProjectionAngle();
	XMMATRIX GetViewMatrix();
	XMFLOAT3 GetPos();

	void ShowHUD(XMMATRIX projection, Light* ambient, DirectionalLight* directional = nullptr, PointLight* point = nullptr);
};

