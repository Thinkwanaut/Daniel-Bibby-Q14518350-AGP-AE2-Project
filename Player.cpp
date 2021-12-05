#include "Player.h"

Player::Player(ID3D11Device* device, ID3D11DeviceContext* context, AssetManager* assets, char* model, char* texture, char* shader)
	: GameObject{ device, context, assets, model, texture, shader }
{
	m_Health = m_MaxHealth;
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

Bullet* Player::Shoot()
{
	XMVECTOR gunPos = XMVector3Normalize(XMVector3Cross({ m_LookX, m_dy, m_LookZ }, -m_up));
	gunPos += XMVector3Normalize(XMVector3Cross(gunPos, { m_LookX, m_dy, m_LookZ, 0}));
	gunPos *= m_GunOffset;
	gunPos += {m_x, m_y, m_z};
	XMVECTOR gunTarget = { XMVectorGetX(gunPos) + m_LookX * m_TargetDist, XMVectorGetY(gunPos) + m_dy * m_TargetDist, XMVectorGetZ(gunPos) + m_LookZ * m_TargetDist };
	Bullet* bullet = new Bullet(mp_D3DDevice, mp_ImmediateContext, mp_Assets, m_BulletModel, m_BulletTexture, (char*)"shaders.hlsl");
	bullet->Shoot(gunPos, gunTarget);
	return bullet;
}

void Player::SpikeCheck(std::vector<GameObject*> spikes)
{
	for (GameObject* spike : spikes)
	{
		if (CheckCollision(spike))
		{
			m_Health = max(m_Health - 1, 0);
			m_Thrown = true;
			XMFLOAT2 throwDir = Normalise2D({ m_x - spike->GetX(), m_z - spike->GetZ() });
			m_ThrowVelX = throwDir.x * m_JumpSpeed;
			m_ThrowVelZ = throwDir.y * m_JumpSpeed;
			MoveToTop(spike);
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
			m_Health = max(m_Health - 1, 0);
			return e;
		}
	}

	return -1;
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
				break;
			case Collectable::KEY:
				m_Score++;
				break;
			default:
				break;
			}
			return i;
		}
	}
	return -1;
}

int Player::Health()
{
	return m_Health;
}

int Player::Score()
{
	return m_Score;
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
