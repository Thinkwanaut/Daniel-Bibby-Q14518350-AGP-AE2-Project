#include "Game.h"
Game::Game(_In_ HINSTANCE hInstance, _In_ int nCmdShow, Window* window)
{
	mp_Window = window;

	mp_Timer = new Timer();

	mp_Assets = new AssetManager(window->Device(), window->Context());
	mp_Assets->LoadModel(CUBE_FILE);
	mp_Assets->LoadModel(SPHERE_FILE);
	mp_Assets->LoadTexture(BOX_FILE);
	mp_Assets->LoadTexture(HEALTH_FILE);
	mp_Assets->LoadTexture(ORB_FILE);
	mp_Assets->LoadShaders(SKY_SHADER_FILE, DRAW_TYPE::Skybox);


	HRESULT hr;
	mp_Input = new Input(mp_Window->HInst(), mp_Window->HWnd(), &hr);
	if (FAILED(hr)) DXTRACE_MSG("Failed to initialise input");

	mp_2DText = new Text2D("Assets/myFont.png", mp_Window->Device(), mp_Window->Context());

	SetLayout();
	CreateLevel();
}

Game::~Game()
{
	DestroyLevel();
	if (mp_Skybox)
	{
		delete mp_Skybox;
		mp_Skybox = nullptr;
	}
	if (mp_PLight)
	{
		delete mp_PLight;
		mp_PLight = nullptr;
	}
	if (mp_DLight)
	{
		delete mp_DLight;
		mp_DLight = nullptr;
	}
	if (mp_ALight)
	{
		delete mp_ALight;
		mp_ALight = nullptr;
	}
	if (mp_2DText)
	{
		delete mp_2DText;
		mp_2DText = nullptr;
	}
	if (mp_Window)
	{
		delete mp_Window;
		mp_Window = nullptr;
	}
}

void Game::DestroyLevel()
{
	for (GameObject* obstacle : mp_Obstacles) delete obstacle;
	mp_Obstacles.clear();

	for (GameObject* movable : mp_Movables) delete movable;
	mp_Movables.clear();

	for (GameObject* spikes : mp_Spikes) delete spikes;
	mp_Spikes.clear();

	for (GameObject* spawn : mp_Spawns) delete spawn;
	mp_Spawns.clear();

	for (Enemy* enemy : mp_Enemies) delete enemy;
	mp_Enemies.clear();

	for (GameObject* key : mp_Keys) delete key;
	mp_Keys.clear();

	for (GameObject* healthPack : mp_HealthPacks) delete healthPack;
	mp_HealthPacks.clear();

	for (Bullet* bullet : mp_Bullets) delete bullet;
	mp_Bullets.clear();

	mp_Blockers.clear();

	if (mp_Player)
	{
		delete mp_Player;
		mp_Player = nullptr;
	}

	mp_Pushers.clear();
}
void Game::SetLayout()
{
	std::ifstream levelFile(LEVEL_FILE);
	std::string row;

	if (levelFile.is_open())
	{
		while (getline(levelFile, row)) //Gets all lines of level file and stores them in a string vector
		{
			m_Layout.push_back(row);
		}
	}
	levelFile.close();
}

void Game::CreateLevel()
{
	DestroyLevel();

	if (m_SingleFloor)
	{
		mp_Obstacles.push_back(new GameObject(mp_Window->Device(), mp_Window->Context(), mp_Assets, CUBE_FILE, FLOOR_FILE, SHADER_FILE));
		mp_Obstacles[0]->SetPos(m_Layout[0].length() * m_BlockSize / 2.0f, m_FloorHeight, m_Layout.size() * m_BlockSize / 2.0f);
		mp_Obstacles[0]->SetScale(m_Layout[0].length() * m_BlockSize / 2.0f, m_BlockSize, m_BlockSize * m_Layout.size() / 2.0f);
		mp_Obstacles[0]->SetCollisionType(ColliderShape::Box);
		mp_Obstacles[0]->SetFall(false);
	}

	for (int row = 0; row < m_Layout.size(); row++)
	{
		int z = m_BlockSize * row;
		for (int column = 0; column < m_Layout[0].length(); column++)
		{
			mp_Window->ResetContext();
			mp_Input->ReadInputStates();
			int x = m_BlockSize * column; //Get game coordinates
			int index = 0;
			int floorHeight = m_FloorHeight;

			if (m_Layout[row][column] != '.')
			{
				//Create all objects in game based on text file loaded externally
				switch (std::toupper(m_Layout[row][column]))
				{
				case (int)ObjectTypes::MOVABLE:
					index = mp_Movables.size();
					mp_Movables.push_back(new GameObject(mp_Window->Device(), mp_Window->Context(), mp_Assets, CUBE_FILE, STONE_FILE, SHADER_FILE));
					mp_Movables[index]->SetPos(x, m_ObjectHeight, z);
					mp_Movables[index]->SetScale(m_BlockSize / 2.5f, m_BlockSize / 2.0f, m_BlockSize / 2.5f);
					mp_Movables[index]->SetCollisionType(ColliderShape::Box);
					break;

				case (int)ObjectTypes::SPIKES:
					index = mp_Spikes.size();
					mp_Spikes.push_back(new GameObject(mp_Window->Device(), mp_Window->Context(), mp_Assets, CUBE_FILE, SPIKE_FILE, SHADER_FILE));
					mp_Spikes[index]->SetPos(x, (m_SingleFloor) ? m_ObjectHeight : m_FloorHeight, z);
					mp_Spikes[index]->SetScale(m_BlockSize / 2.0f, m_BlockSize / 4.0f, m_BlockSize / 2.0f);
					mp_Spikes[index]->SetCollisionType(ColliderShape::Box);
					floorHeight -= m_BlockSize;
					break;

				case (int)ObjectTypes::HEALTH:
					index = mp_HealthPacks.size();
					mp_HealthPacks.push_back(new GameObject(mp_Window->Device(), mp_Window->Context(), mp_Assets, CUBE_FILE, HEALTH_FILE, SHADER_FILE));
					mp_HealthPacks[index]->SetPos(x, m_ObjectHeight, z);
					mp_HealthPacks[index]->SetScale(m_BlockSize / 4.0f, m_BlockSize / 4.0f, m_BlockSize / 4.0f);
					mp_HealthPacks[index]->SetCollisionType(ColliderShape::Box);
					break;

				case (int)ObjectTypes::ENEMY:
					index = mp_Spawns.size();
					mp_Spawns.push_back(new GameObject(mp_Window->Device(), mp_Window->Context(), mp_Assets, SPHERE_FILE, SPIKE_FILE, SHADER_FILE));
					mp_Spawns[index]->SetColour({ 1, 0, 0, 1 });
					mp_Spawns[index]->SetScale(m_BlockSize / 2.0f, m_BlockSize / 2.0f, m_BlockSize / 2.0f);
					mp_Spawns[index]->SetPos(x, m_ObjectHeight + abs(m_FloorHeight / 2.0f), z);
					break;

				case (int)ObjectTypes::PLAYER:
					mp_Player = new Player(mp_Window->Device(), mp_Window->Context(), mp_Assets, CUBE_FILE, BOX_FILE, SHADER_FILE);
					mp_Player->SetPos(x, m_ObjectHeight, z);
					mp_Player->SetScale(m_BlockSize * 0.1f, m_BlockSize * 1.25f, m_BlockSize * 0.1f);
					mp_Player->SetCollisionType(ColliderShape::Box);
					mp_Player->SetBullet(SPHERE_FILE, BLANK_FILE);
					break;

				default:
					break;
				}
			}
			if (!m_SingleFloor)
			{
				index = mp_Obstacles.size();
				mp_Obstacles.push_back(new GameObject(mp_Window->Device(), mp_Window->Context(), mp_Assets, CUBE_FILE, FLOOR_FILE, SHADER_FILE));
				mp_Obstacles[index]->SetPos(x, floorHeight, z);
				mp_Obstacles[index]->SetScale(m_BlockSize / 2.0f, m_BlockSize / 2.0f, m_BlockSize / 2.0f);
				mp_Obstacles[index]->SetCollisionType(ColliderShape::Box);
				mp_Obstacles[index]->SetFall(false);
			}

			if ((column == 0 || column == m_Layout[0].length() - 1) && row == m_Layout.size() / 2)
			{
				index = mp_Obstacles.size();
				mp_Obstacles.push_back(new GameObject(mp_Window->Device(), mp_Window->Context(), mp_Assets, CUBE_FILE, FENCE_FILE, SHADER_FILE));
				mp_Obstacles[index]->SetPos(x, m_FloorHeight, z);
				mp_Obstacles[index]->SetScale(m_BlockSize / 2.0f, m_BlockSize * 5.0f, m_BlockSize * m_Layout.size() / 2.0f);
				mp_Obstacles[index]->SetCollisionType(ColliderShape::Box);
				mp_Obstacles[index]->SetFall(false);
			}

			mp_Window->Clear( 0.0f, 0.0f, 0.0f );

			int total = m_Layout.size() * m_Layout[0].length();
			int completed = row * m_Layout[0].length() + column;
			int percent = (int)((float)completed / (float)total * 100);
			mp_2DText->AddText(std::to_string(percent) + "%", 0, 0, 0.2, { 0, 1, 1, 1 }, Alignment::Centre);
			mp_2DText->RenderText();
			mp_Window->Present();

			if (mp_Input->KeyPressed(KEYS::ESC)) return;
		}
		if (row == 0 || row == m_Layout.size() - 1)
		{
			int index = mp_Obstacles.size();
			mp_Obstacles.push_back(new GameObject(mp_Window->Device(), mp_Window->Context(), mp_Assets, CUBE_FILE, FENCE_FILE, SHADER_FILE));
			mp_Obstacles[index]->SetPos(m_BlockSize * m_Layout[0].length() / 2.0f, m_FloorHeight, z);
			mp_Obstacles[index]->SetScale(m_BlockSize * m_Layout[0].length() / 2.0f, m_BlockSize * 5.0f, m_BlockSize / 2.0f);
			mp_Obstacles[index]->SetCollisionType(ColliderShape::Box);
			mp_Obstacles[index]->SetFall(false);
		}

	}

	mp_Pushers.insert(mp_Pushers.end(), mp_Movables.begin(), mp_Movables.end());
	mp_Pushers.push_back(mp_Player);

	mp_Blockers.insert(mp_Blockers.end(), mp_Obstacles.begin(), mp_Obstacles.end());
	mp_Blockers.insert(mp_Blockers.end(), mp_Movables.begin(), mp_Movables.end());

	mp_Skybox = new Skybox(mp_Window->Device(), mp_Window->Context(), mp_Assets, SKY_FILE, (char*)"skyShaders.hlsl");
	mp_Skybox->SetScale(3, 3, 3);

	m_GameEnded = false;

	mp_Timer->Tick();
	SpawnEnemies();
}

int Game::Run()
{
	//Main Message Loop
	MSG msg = { 0 };

	while (msg.message != WM_QUIT)
	{
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		else Update();
	}

	return (int)msg.wParam;
}

void Game::MovePlayer(float adjust)
{
	mp_Player->Move(mp_Input, mp_Obstacles, m_Gravity, adjust);
	mp_Player->SetGun(mp_Input);
	if (mp_Input->MouseButtonHeld(MOUSE::LCLICK))
	{
		std::vector<Bullet*> newBullets = mp_Player->Shoot();
		mp_Bullets.insert(mp_Bullets.begin(), newBullets.begin(), newBullets.end());
	}
	int enemyHit = mp_Player->EnemyCheck(mp_Enemies);
	if (enemyHit >= 0) mp_Enemies.erase(std::begin(mp_Enemies) + enemyHit);

	int healthpackHit = mp_Player->CollectItem(mp_HealthPacks, Collectable::HEALTH);
	if (healthpackHit >= 0) mp_HealthPacks.erase(std::begin(mp_HealthPacks) + healthpackHit);

	int keyHit = mp_Player->CollectItem(mp_Keys, Collectable::KEY);
	if (keyHit >= 0) mp_Keys.erase(std::begin(mp_Keys) + keyHit);

	mp_Player->SpikeCheck(mp_Spikes);

	//mp_PLight->SetPosition({ mp_Player->GetX(), mp_Player->GetY(), mp_Player->GetZ() });

	if (mp_Player->Dead())
	{
		m_GameEnded = true;
		m_PauseText = "DEFEAT";
	}

	else if (mp_Player->Won())
	{
		m_GameEnded = true;
		m_PauseText = "VICTORY";
	}

}

void Game::MoveBullets(float fpsAdjustment)
{
	int bulletCounter = 0;
	while (bulletCounter != mp_Bullets.size())
	{
		if (mp_Bullets[bulletCounter]->Move(mp_Blockers, fpsAdjustment)) mp_Bullets.erase(std::begin(mp_Bullets) + bulletCounter);
		else
		{
			int targetHit;
			bool destroyBullet = mp_Bullets[bulletCounter]->TargetCheck(mp_Enemies, &targetHit);
			if (targetHit >= 0)
			{
				if (mp_Enemies[targetHit]->IsDead())
				{
					mp_Keys.push_back(mp_Enemies[targetHit]->SpawnKey());
					mp_Enemies.erase(std::begin(mp_Enemies) + targetHit); //Delete enemy the bullet has hit
				}
			}
			if (destroyBullet) mp_Bullets.erase(std::begin(mp_Bullets) + bulletCounter); //Delete bullet registering collision
			else ++bulletCounter; //Allow bullets to be deleted mid-loop without out-indexing vector
		}
	}
}

XMMATRIX Game::XYZRotation(float x, float y, float z)
{
	XMMATRIX xMatrix, yMatrix, zMatrix;
	xMatrix = XMMatrixRotationX(XMConvertToRadians(x));
	yMatrix = XMMatrixRotationY(XMConvertToRadians(y));
	zMatrix = XMMatrixRotationZ(XMConvertToRadians(z));
	return xMatrix * yMatrix * zMatrix;
}

void Game::SpawnEnemies()
{
	for (GameObject* spawn : mp_Spawns)
	{
		int index = mp_Enemies.size();
		mp_Enemies.push_back(new Enemy(mp_Window->Device(), mp_Window->Context(), mp_Assets, CUBE_FILE, ENEMY_FILE, SHADER_FILE));
		mp_Enemies[index]->SetPos(spawn->GetPos());
		mp_Enemies[index]->SetKey(SPHERE_FILE, ORB_FILE);
	}
	mp_Timer->StartTimer("Spawn");
}

void Game::Update()
{
	if (mp_Window->Resized()) mp_Timer->Tick();
	float adjust = mp_Timer->Tick() * m_BlockSize;
	mp_Input->ReadInputStates();
	mp_Window->ResetContext();

	if (mp_Input->KeyPressed(KEYS::P) && !m_GameEnded)
	{
		m_Paused = !m_Paused;
		m_PauseText = "PAUSED-(P)";
	}

	if (!m_Paused && !m_GameEnded)
	{
		mp_DLight->Rotate({ 0, adjust / m_BlockSize, 0 });

		if (mp_Timer->GetTimer("Spawn") >= m_EnemyInterval)	SpawnEnemies();
		MovePlayer(adjust);
		MoveBullets(adjust);
		for (GameObject* m : mp_Movables)
		{
			m->Fall(mp_Obstacles, m_Gravity, adjust);
			m->GetPushed(mp_Pushers, mp_Obstacles);
		}
		for (Enemy* e : mp_Enemies) e->Move(mp_Player, mp_Blockers, adjust);
		for (GameObject* h : mp_HealthPacks) h->Fall(mp_Obstacles, m_Gravity, adjust);
		for (GameObject* k : mp_Keys) k->Fall(mp_Obstacles, m_Gravity, adjust);
		for (GameObject* s : mp_Spikes) s->Fall(mp_Obstacles, m_Gravity, adjust);
		XMFLOAT3 camPos = mp_Player->GetPos();
		mp_Skybox->SetPos(camPos.x, camPos.y + m_SkyOffset, camPos.z);

		Draw();
	}
	else
	{
		mp_Window->Rainbow(adjust / m_BlockSize);
		mp_Window->Clear();
		mp_2DText->AddText(m_PauseText, 0, 0, .15, { 0, 0, 0, 1 }, Alignment::Centre);
		if (m_GameEnded) mp_2DText->AddText("PRESS-ENTER-TO-RESTART", 0, -.25, .05, { 0, 0, 0, 1 }, Alignment::Centre);
		mp_2DText->RenderText();
		mp_Window->Present();

		if (mp_Input->KeyPressed(KEYS::ENTER) && m_GameEnded) CreateLevel();
	}
	
}

void Game::Draw()
{
	XMMATRIX projection = XMMatrixPerspectiveFovLH(XMConvertToRadians(mp_Player->GetProjectionAngle()), mp_Window->Width() / mp_Window->Height(), 0.1f, 1000.0f);

	mp_Skybox->Draw(mp_Player->GetViewMatrix(), projection);

	for (GameObject* o : mp_Obstacles) o->Draw(mp_Player->GetViewMatrix(), projection, mp_ALight, mp_DLight);
	for (GameObject* s : mp_Spawns) s->Draw(mp_Player->GetViewMatrix(), projection, mp_ALight, mp_DLight);
	for (Bullet* b : mp_Bullets) b->Draw(mp_Player->GetViewMatrix(), projection, mp_ALight, mp_DLight);
	for (Enemy* e : mp_Enemies) e->Draw(mp_Player->GetViewMatrix(), projection, mp_ALight, mp_DLight);
	for (GameObject* h : mp_HealthPacks) h->Draw(mp_Player->GetViewMatrix(), projection, mp_ALight, mp_DLight);
	for (GameObject* k : mp_Keys) k->Draw(mp_Player->GetViewMatrix(), projection, mp_ALight, mp_DLight);
	for (GameObject* m : mp_Movables) m->Draw(mp_Player->GetViewMatrix(), projection, mp_ALight, mp_DLight);
	for (GameObject* s : mp_Spikes) s->Draw(mp_Player->GetViewMatrix(), projection, mp_ALight, mp_DLight);

	mp_Player->ShowHUD(projection, mp_ALight, mp_DLight);

	mp_Window->Present();
}

#pragma region MATHS

XMFLOAT2 Game::PixelToVertex(XMFLOAT2 pixelCoords)
{
	float vertX = (pixelCoords.x / (float)mp_Window->Width() - 0.5f) * 2;
	float vertY = (-pixelCoords.y / (float)mp_Window->Height() + 0.5f) * 2;

	return XMFLOAT2(vertX, vertY);
}

XMFLOAT2 Game::VertexToPixel(XMFLOAT2 vertexCoords)
{
	float pixX = ((vertexCoords.x / 2.0f) + 0.5f) + mp_Window->Width();
	float pixY = ((vertexCoords.y / 2.0f) + 0.5f) + mp_Window->Height();

	return XMFLOAT2(pixX, pixY);
}

#pragma endregion
