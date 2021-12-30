#include "Player.h"

Player::Player(ID3D11Device* device, ID3D11DeviceContext* context, AssetManager* assets, char* model, char* texture, char* shader)
	: GameObject{ device, context, assets, model, texture, shader }
{
	m_position = XMVectorSet(m_x, m_y, m_z, 0.0);
	m_lookAt = m_position + XMVectorSet(m_LookX, m_dy, m_LookZ, 0.0f);
	m_up = XMVectorSet(0.0, 1.0, 0.0, 0.0);

	m_HealthColour = m_DefaultTextColour; m_ScoreColour = m_DefaultTextColour; m_SpikeColour = m_DefaultTextColour;

	m_Health = m_MaxHealth;
	mp_2DText = new Text2D("Assets/myFont.png", device, context);
	mp_Timer = new Timer();

	/*struct Gun
	{
		ShotInterval
		HipAccuracy
		ZoomAccuracy
		BulletSpeed
		Range
		Damage 
		ShotNum
		ThroughEnemies
		HipCrosshair
		ZoomCrosshair
		ZoomAngle
	};*/
	
	// MAKE YOUR OWN :)
	m_Guns.push_back(Gun{ 0.02f, 0.8f, 0.95f, 0.75f, 100.0f, 1, 1, false, "( )", "()", 60.0f});
	m_Guns.push_back(Gun{ 0.25f, 0.5f, 0.75f, 0.5f, 50.0f, 1, 25, false, "(  )", "( )", 70.0f});
	m_Guns.push_back(Gun{ 1.0f, 0.8f, 1.0f, 5.f, 500.0f, 10, 1, true, "[ ]", "[+]", 45.0f});

	for (int g = 0; g < m_Guns.size(); g++)	mp_Timer->StartTimer("Shoot" + std::to_string(g));

	mp_GunModel = new GameObject(device, context, assets, (char*)"Assets/Cube.obj", (char*)"Assets/BlankTexture.bmp", (char*)"shaders.hlsl");
	mp_GunModel->SetScale(0.5f, 0.5f, 1.0f);
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

void Player::Move(Input* input, std::vector<GameObject*> obstacles, float floorHeight, float grav, float adjust)
{
	AddYRot(input->MouseRelX());
	AddXRot(-input->MouseRelY());

	m_Thrown = m_Thrown && !m_Grounded;

	if (!m_Thrown)
	{
		XMFLOAT2 moveVec{ 0, 0 };
		float sprint{ 1 };

		if (input->KeyHeld(KEYS::W) || input->KeyHeld(KEYS::UP)) moveVec.x = 1;
		if (input->KeyHeld(KEYS::S) || input->KeyHeld(KEYS::DOWN)) moveVec.x -= 1;
		if (input->KeyHeld(KEYS::D) || input->KeyHeld(KEYS::RIGHT)) moveVec.y = 1;
		if (input->KeyHeld(KEYS::A) || input->KeyHeld(KEYS::LEFT)) moveVec.y -= 1;
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
			if (CheckCollision(obstacle)) m_x -= m_ThrowVelX * adjust;
		
		m_z += m_ThrowVelZ * adjust;
		for (GameObject* obstacle : obstacles)
			if (CheckCollision(obstacle)) m_z -= m_ThrowVelZ * adjust;
	}
	Fall(obstacles, floorHeight, grav, adjust);

	m_position = XMVectorSet(m_x, m_y, m_z, 0.0);
	m_lookAt = m_position + XMVectorSet(m_LookX, m_dy, m_LookZ, 0.0f);
	m_up = XMVectorSet(0.0, 1.0, 0.0, 0.0);
}

#pragma region MOVEMENT
void Player::Forward(float movement, std::vector<GameObject*> obstacles)
{
	m_x += m_dx * movement * m_speed;
	for (GameObject* obstacle : obstacles)
		if (CheckCollision(obstacle)) m_x -= m_dx * movement * m_speed;
	
	m_z += m_dz * movement * m_speed;
	for (GameObject* obstacle : obstacles)
		if (CheckCollision(obstacle)) m_z -= m_dz * movement * m_speed;
}

void Player::Strafe(float movement, std::vector<GameObject*> obstacles)
{
	m_x += m_dz * movement * m_speed;
	for (GameObject* obstacle : obstacles)
		if (CheckCollision(obstacle)) m_x -= m_dz * movement * m_speed;
	
	m_z -= m_dx * movement * m_speed;
	for (GameObject* obstacle : obstacles)
		if (CheckCollision(obstacle)) m_z += m_dx * movement * m_speed;
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

	m_LookX = m_dx * cos(XMConvertToRadians(min(max(m_CamRotX, -m_AngleClamp), m_AngleClamp)));
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
	return mp_Timer->GetTimer("Shoot" + std::to_string(m_GunIndex)) > m_Guns[m_GunIndex].ShotInterval;
}

float Player::GetRandTarget()
{
	// 1 += accuracy
	float accuracy = m_Zoomed ? m_Guns[m_GunIndex].ZoomAccuracy : m_Guns[m_GunIndex].HipAccuracy;
	return RandomPN(1.0f - accuracy);
}

void Player::SetGun(Input* input)
{
	int cycle = input->MouseWheel();
	if (input->KeyPressed(KEYS::E)) cycle--;
	if (input->KeyPressed(KEYS::Q)) cycle++;
	if (cycle != 0)
	{
		m_GunIndex += (cycle > 0) ? -1 : 1; // Reverse registered scroll wheel direction, ignore magnitudes greater than 1

		// Cast size to int as size_t cannot be compared to negatives properly i.e., negative numbers are greater than it
		if (m_GunIndex >= (int)m_Guns.size()) m_GunIndex %= m_Guns.size();
		else if (m_GunIndex < 0) m_GunIndex -= m_Guns.size() * (int)floorf((float)m_GunIndex / (float)m_Guns.size());
	}

	m_Zoomed = input->MouseButtonHeld(MOUSE::RCLICK);

	XMFLOAT3 offset = (m_Zoomed) ? m_ZoomOffset : m_HipOffset;

	// Using m_lookAt and m_up caused access violation error only in Release x86. Unsure why
	XMVECTOR gunPos = XMVector3Normalize(XMVector3Cross({ m_LookX, m_dy, m_LookZ }, { 0, -1, 0 })) * offset.x; // Side offset
	gunPos += XMVector3Normalize(XMVector3Cross(gunPos, { m_LookX, m_dy, m_LookZ, 0 })) * offset.y; // Downward offset
	gunPos += XMVector3Normalize({ m_LookX, m_dy, m_LookZ, 0 }) * offset.z; // Forward offset
	gunPos += {m_x, m_y, m_z};

	mp_GunModel->SetPos(XMVectorGetX(gunPos), XMVectorGetY(gunPos), XMVectorGetZ(gunPos));
	mp_GunModel->LookAtRelative(m_LookX, m_dy, m_LookZ);
}

std::vector<Bullet*> Player::Shoot(float gap)
{
	std::vector<Bullet*> newBullets;
	if (ShotReady())
	{
		int totalShots = m_Guns[m_GunIndex].ShotNum;
		if (gap > 0) totalShots *= ceilf(gap / m_Guns[m_GunIndex].ShotInterval); // Spawn delayed in single frame to account for lag
		for (int s = 0; s < totalShots; s++)
		{
			float tx = mp_GunModel->GetX() + m_LookX * m_TargetDist + m_TargetDist * GetRandTarget();
			float ty = mp_GunModel->GetY() + m_dy * m_TargetDist + m_TargetDist * GetRandTarget();
			float tz = mp_GunModel->GetZ() + m_LookZ * m_TargetDist + m_TargetDist * GetRandTarget();
			XMFLOAT3 gunTarget = { tx, ty, tz };
			XMFLOAT3 bullPos = { m_LookX * mp_GunModel->GetScale().z + mp_GunModel->GetPos().x, m_dy * mp_GunModel->GetScale().z + mp_GunModel->GetPos().y, m_LookZ * mp_GunModel->GetScale().z + mp_GunModel->GetPos().z };
			newBullets.push_back(new Bullet(mp_D3DDevice, mp_ImmediateContext, mp_Assets, m_BulletModel, m_BulletTexture, (char*)"shaders.hlsl"));
			newBullets[s]->Shoot(bullPos, gunTarget, m_Guns[m_GunIndex].BulletSpeed, m_Guns[m_GunIndex].Damage, m_Guns[m_GunIndex].Range, m_Guns[m_GunIndex].ThroughEnemies);
			newBullets[s]->MoveForward(m_Guns[m_GunIndex].BulletSpeed * floorf((float)s / (float)m_Guns[m_GunIndex].ShotNum)); // Move catch up bullets forward
		}

		mp_Timer->StartTimer("Shoot" + std::to_string(m_GunIndex));
	}

	return newBullets;
}

void Player::ShowHit()
{
	mp_Timer->StartTimer("Indicator");
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

void Player::SetSpikeScore(int spikes, int maxSpikes)
{
	if (spikes != m_SpikeScore)
	{
		m_SpikeColour = { 1.0f, 1.0f, 0.0f, 1.0f };
		mp_Timer->StartTimer("SpikeFlash");
	}
	m_SpikeScore = spikes;
	m_MaxSpikes = maxSpikes;
}

void Player::Magnet(std::vector<GameObject*> items, float adjust)
{
	for (GameObject* i : items) //(int i = 0; i < items.size(); i++)
	{
		float distRatio = GetDistanceSquared(i) / m_PullRangeSq;
		if (distRatio < 1.0f) i->MoveTowards(this, min(m_MinPull / distRatio, m_MaxPull), adjust);
	}
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
	return m_Score >= m_MaxScore && m_SpikeScore >= m_MaxSpikes;
}

float Player::GetProjectionAngle()
{
	return m_Zoomed ? m_Guns[m_GunIndex].ZoomAngle : m_DefaultProjectionAngle;
}

XMMATRIX Player::GetViewMatrix()
{
	return XMMatrixLookAtLH(m_position, m_lookAt, m_up);
}

XMFLOAT3 Player::GetPos()
{
	return { m_x, m_y, m_z };
}

void Player::ShowHUD(XMMATRIX projection, Light* ambient, DirectionalLight* directional, PointLight* point)
{
	mp_GunModel->Draw(GetViewMatrix(), projection, ambient, directional, point);

	if (mp_Timer->GetTimer("HealthFlash") > m_FlashTime) m_HealthColour = m_DefaultTextColour;
	if (mp_Timer->GetTimer("ScoreFlash") > m_FlashTime) m_ScoreColour = m_DefaultTextColour;
	if (mp_Timer->GetTimer("SpikeFlash") > m_FlashTime) m_SpikeColour = m_DefaultTextColour;

	//Will use text for basic UI images
	mp_2DText->AddText(m_Zoomed ? m_Guns[m_GunIndex].ZoomCrosshair : m_Guns[m_GunIndex].HipCrosshair, 0.0f, 0.0f, 0.1f, { 0.0f, 1.0f, 0.0f, m_Zoomed ? 0.9f : 0.5f }, Alignment::Centre);
	if (0 < mp_Timer->GetTimer("Indicator") && mp_Timer->GetTimer("Indicator") < m_FlashTime / 2.0f)
		mp_2DText->AddText("><", 0.0f, 0.0f, 0.1f, { 1.0f, 1.0f, 0.0f, 0.75f }, Alignment::Centre);
	
	mp_2DText->AddText("HP-" + std::to_string((int)m_Health), -0.9f, 0.9f, 0.05f, m_HealthColour);
	mp_2DText->AddText("Score-" + std::to_string(m_Score) + "/" + std::to_string(m_MaxScore), -0.9f, 0.8f, 0.05f, m_ScoreColour);
	mp_2DText->AddText("Spikes-" + std::to_string(m_SpikeScore) + "/" + std::to_string(m_MaxSpikes), -0.9f, 0.7f, 0.05f, m_SpikeColour);
	mp_2DText->AddText("Gun-" + std::to_string(m_GunIndex), 0.9f, 0.9f, 0.05f, m_DefaultTextColour, Alignment::Right);
	mp_2DText->RenderText();
}
