#include "Player.h"

Player::Player(ID3D11Device* device, ID3D11DeviceContext* context, AssetManager* assets, char* model, char* texture, char* shader)
	: GameObject{ device, context, assets, model, texture, shader }
{
	m_Health = m_MaxHealth;
	mp_2DText = new Text2D("Assets/myFont.png", device, context);
	mp_Timer = new Timer();
	mp_Timer->StartTimer("Shoot");
}

Player::~Player()
{
	if (mp_Timer)
	{
		delete mp_Timer;
		mp_Timer = nullptr;
	}
	if (mp_2DText)
	{
		delete mp_2DText;
		mp_2DText = nullptr;
	}
}

void Player::Move(Input* input, std::vector<GameObject*> obstacles, float grav, float adjust)
{
	AddYRot(input->MouseRelX());
	AddXRot(-input->MouseRelY());

	m_Thrown = m_Thrown && !m_Grounded;

	if (!m_Thrown)
	{
		XMFLOAT2 moveVec{ 0, 0 };
		float sprint{ 1 };

		if (input->KeyHeld(KEYS::W)) moveVec.x = 1;
		if (input->KeyHeld(KEYS::S)) moveVec.x -= 1;
		if (input->KeyHeld(KEYS::D)) moveVec.y = 1;
		if (input->KeyHeld(KEYS::A)) moveVec.y -= 1;
		if (input->KeyPressed(KEYS::SPACE)) Jump();

		if (input->KeyHeld(KEYS::LSHIFT) || input->KeyHeld(KEYS::RSHIFT)) sprint *= m_Sprint;

		moveVec = Normalise2D(moveVec);

		if (moveVec.x != 0) Forward(moveVec.x * sprint * adjust, obstacles);
		if (moveVec.y != 0) Strafe(moveVec.y * sprint * adjust, obstacles);
	}

	else
	{
		m_x += m_ThrowVelX * adjust;
		for (GameObject* obstacle : obstacles)
		{
			if (CheckCollision(obstacle)) m_x -= m_ThrowVelX * adjust;
		}
		m_z += m_ThrowVelZ * adjust;
		for (GameObject* obstacle : obstacles)
		{
			if (CheckCollision(obstacle)) m_z -= m_ThrowVelZ * adjust;
		}
	}
	Fall(obstacles, grav, adjust);
}

#pragma region MOVEMENT
void Player::Forward(float movement, std::vector<GameObject*> obstacles)
{
	m_x += m_dx * movement * m_speed;
	for (GameObject* obstacle : obstacles)
	{
		if (CheckCollision(obstacle)) m_x -= m_dx * movement * m_speed;
	}

	m_z += m_dz * movement * m_speed;
	for (GameObject* obstacle : obstacles)
	{
		if (CheckCollision(obstacle)) m_z -= m_dz * movement * m_speed;
	}
}

void Player::Strafe(float movement, std::vector<GameObject*> obstacles)
{
	XMVECTOR right = XMVector3Normalize(XMVector3Cross(m_position - m_lookAt, m_up));

	m_x += XMVectorGetX(right) * movement * m_speed;
	for (GameObject* obstacle : obstacles)
	{
		if (CheckCollision(obstacle)) m_x -= XMVectorGetX(right) * movement * m_speed;
	}
	m_z += XMVectorGetZ(right) * movement * m_speed;
	for (GameObject* obstacle : obstacles)
	{
		if (CheckCollision(obstacle)) m_z -= XMVectorGetZ(right) * movement * m_speed;
	}
}

void Player::Fly(float yAdd)
{
	m_y += yAdd;
}

void Player::Jump(bool jumpAnyway)
{
	if (m_Grounded || jumpAnyway)
	{
		m_FallVel -= m_JumpSpeed;
		m_Grounded = false;
	}
}

void Player::SetXRot(float xRot)
{
	m_CamRotX = xRot;
	SetCameraLook();
}

void Player::SetYRot(float yRot)
{
	m_CamRotY = yRot;
	SetCameraLook();
}

void Player::AddXRot(float xAdd)
{
	m_CamRotX += xAdd;
	SetCameraLook();
}

void Player::AddYRot(float yAdd)
{
	m_CamRotY += yAdd;
	SetCameraLook();
}

void Player::SetCameraLook()
{
	m_dx = sin(XMConvertToRadians(m_CamRotY));
	m_dz = cos(XMConvertToRadians(m_CamRotY));
	m_dy = sin(XMConvertToRadians(min(max(m_CamRotX, -m_AngleClamp), m_AngleClamp)));

	m_LookX = m_dx* cos(XMConvertToRadians(min(max(m_CamRotX, -m_AngleClamp), m_AngleClamp)));
	m_LookZ = m_dz * cos(XMConvertToRadians(min(max(m_CamRotX, -m_AngleClamp), m_AngleClamp)));
}

#pragma endregion

void Player::SetBullet(char* model, char* texture)
{
	m_BulletModel = model;
	m_BulletTexture = texture;
}

bool Player::ShotReady()
{
	return mp_Timer->GetTimer("Shoot") > m_ShotInterval;
}

float Player::GetRandTarget()
{
	return ((float)(rand() % (int)((1 - m_Accuracy) * 100 + 1)) / 100.0f) - (1 - m_Accuracy) / 2.0f;
}

std::vector<Bullet*> Player::Shoot()
{
	std::vector<Bullet*> newBullets;
	if (ShotReady())
	{
		for (int s = 0; s < m_ShotNum; s++)
		{
			XMVECTOR gunPos = XMVector3Normalize(XMVector3Cross({ m_LookX, m_dy, m_LookZ }, -m_up));
			gunPos += XMVector3Normalize(XMVector3Cross(gunPos, { m_LookX, m_dy, m_LookZ, 0}));
			gunPos *= m_GunOffset;
			gunPos += {m_x, m_y, m_z};
			float tx = XMVectorGetX(gunPos) + m_LookX * m_TargetDist + m_TargetDist * GetRandTarget();
			float ty = XMVectorGetY(gunPos) + m_dy * m_TargetDist + m_TargetDist * GetRandTarget();
			float tz = XMVectorGetZ(gunPos) + m_LookZ * m_TargetDist + m_TargetDist * GetRandTarget();
			XMVECTOR gunTarget = { tx, ty, tz };
			newBullets.push_back(new Bullet(mp_D3DDevice, mp_ImmediateContext, mp_Assets, m_BulletModel, m_BulletTexture, (char*)"shaders.hlsl"));
			newBullets[s]->Shoot(gunPos, gunTarget);
		}

		mp_Timer->StartTimer("Shoot");
	}

	return newBullets;
}

void Player::SpikeCheck(std::vector<GameObject*> spikes)
{
	for (GameObject* spike : spikes)
	{
		if (CheckCollision(spike))
		{
			GetHit();
			m_Thrown = true;
			XMFLOAT2 throwDir = Normalise2D({ m_x - spike->GetX(), m_z - spike->GetZ() });
			m_ThrowVelX = throwDir.x * m_JumpSpeed / 2.0f;
			m_ThrowVelZ = throwDir.y * m_JumpSpeed / 2.0f;
			MoveToEdgeY(spike);
			Jump(true);
		}
	}
}

int Player::EnemyCheck(std::vector<Enemy*> enemies)
{
	for (int e = 0; e < enemies.size(); e++)
	{
		if (CheckCollision(enemies[e]))
		{
			GetHit();
			return e;
		}
	}

	return -1;
}

void Player::GetHit()
{
	m_Health = max(m_Health - 1, 0);
	m_HealthColour = { 1.0f, 0.0f, 0.0f, 1.0f };
	mp_Timer->StartTimer("HealthFlash");
}

int Player::CollectItem(std::vector<GameObject*> items, Collectable itemType)
{
	for (int i = 0; i < items.size(); i++)
	{
		if (CheckCollision(items[i]))
		{
			switch (itemType)
			{
			case Collectable::HEALTH:
				m_Health = min(m_Health + 1, m_MaxHealth);
				m_HealthColour = { 0.0f, 1.0f, 0.0f, 1.0f };
				mp_Timer->StartTimer("HealthFlash");
				break;
			case Collectable::KEY:
				m_Score++;
				m_ScoreColour = { 1.0f, 1.0f, 0.0f, 1.0f };
				mp_Timer->StartTimer("ScoreFlash");
				break;
			default:
				break;
			}
			return i;
		}
	}
	return -1;
}

bool Player::Dead()
{
	return m_Health <= 0;
}

bool Player::Won()
{
	return m_Score >= m_MaxScore;
}

XMMATRIX Player::GetViewMatrix()
{
	m_position = XMVectorSet(m_x, m_y, m_z, 0.0);
	m_lookAt = m_position + XMVectorSet(m_LookX, m_dy, m_LookZ, 0.0f);
	m_up = XMVectorSet(0.0, 1.0, 0.0, 0.0);

	return XMMatrixLookAtLH(m_position, m_lookAt, m_up);
}

XMFLOAT3 Player::GetPos()
{
	return { m_x, m_y, m_z };
}

XMFLOAT2 Player::Normalise2D(XMFLOAT2 vector)
{
	float magnitude = powf(powf(vector.x, 2.0f) + powf(vector.y, 2), 0.5f);
	if (magnitude > 0)
		return XMFLOAT2{ vector.x / magnitude, vector.y / magnitude };
	else return vector;
}

void Player::ShowUI()
{
	if (mp_Timer->GetTimer("HealthFlash") > m_FlashTime) m_HealthColour = { 1.0f, 1.0f, 1.0f, 1.0f };
	if (mp_Timer->GetTimer("ScoreFlash") > m_FlashTime) m_ScoreColour = { 1.0f, 1.0f, 1.0f, 1.0f };

	mp_2DText->AddText("(+)", 0.0f, 0.0f, 0.1f, { 0.0f, 1.0f, 0.0f, 0.7f }, true); //Will use text for basic UI images
	mp_2DText->AddText("HP-" + std::to_string((int)m_Health), -0.9f, 0.9f, 0.05f, m_HealthColour);
	mp_2DText->AddText("Score-" + std::to_string(m_Score) + "/" + std::to_string(m_MaxScore), -0.9f, 0.8f, 0.05f, m_ScoreColour);
	mp_2DText->RenderText();
}
