#include "Game.h"
Game::Game(_In_ HINSTANCE hInstance, _In_ int nCmdShow, Window* window)
{
#ifndef NDEBUG
	m_DebugMode = true; // Use for optimisation options
#endif

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
		mp_Obstacles[0]->SetScale(m_Layout[0].length() * m_BlockSize / 2.0f, m_BlockSize / 2.0f, m_BlockSize * m_Layout.size() / 2.0f);
		mp_Obstacles[0]->SetCollisionType(ColliderShape::Box);
		mp_Obstacles[0]->SetFall(false);
	}

	for (int row = 0; row < m_Layout.size(); row++)
	{
		int z = m_BlockSize * row;
		// Check if row string does not contain spike character
		bool singleRow = m_DebugMode && m_Layout[row].find((char)ObjectTypes::SPIKES) == m_Layout[row].npos; 
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
					if (!m_DebugMode) mp_Spikes[index]->MakeParticles(Presets::FIRE);
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
					mp_Spawns[index]->SetTint({ 1, 0, 0, 1 });
					mp_Spawns[index]->AddTintCycle({ 1, 1, 0, 1 });
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
			if (!m_SingleFloor && !singleRow)
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
		if (!m_SingleFloor && singleRow)
		{
			int index = mp_Obstacles.size();
			mp_Obstacles.push_back(new GameObject(mp_Window->Device(), mp_Window->Context(), mp_Assets, CUBE_FILE, FLOOR_ROW_FILE, SHADER_FILE));
			mp_Obstacles[index]->SetPos(m_BlockSize* m_Layout[0].length() / 2.0f, m_FloorHeight, z);
			mp_Obstacles[index]->SetScale(m_Layout[0].length() * m_BlockSize / 2.0f, m_BlockSize / 2.0f, m_BlockSize / 2.0f);
			mp_Obstacles[index]->SetCollisionType(ColliderShape::Box);
			mp_Obstacles[index]->SetFall(false);
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

	m_State = GameStates::PLAY;

	SpawnEnemies();
	mp_Timer->TickFPS();

	for (GameObject* s : mp_Spikes) s->StartParticles();
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

bool Game::InGame()
{
	return m_State == GameStates::PLAY || m_State == GameStates::PAUSE;
}

void Game::MovePlayer()
{
	mp_Player->Move(mp_Input, mp_Obstacles, m_FloorHeight, m_Gravity, m_SpeedAdjust);
	mp_Player->SetGun(mp_Input);
	if (mp_Input->MouseButtonHeld(MOUSE::LCLICK) || mp_Input->PadAxis(AXIS::RTRIGGER) != 0.0f)
	{
		std::vector<Bullet*> newBullets = mp_Player->Shoot(m_FrameGap);
		mp_Bullets.insert(mp_Bullets.begin(), newBullets.begin(), newBullets.end());
	}
	int enemyHit = mp_Player->EnemyCheck(mp_Enemies);
	if (enemyHit >= 0) mp_Enemies.erase(std::begin(mp_Enemies) + enemyHit);

	int healthpackHit = mp_Player->CollectItem(mp_HealthPacks, Collectable::HEALTH);
	if (healthpackHit >= 0) mp_HealthPacks.erase(std::begin(mp_HealthPacks) + healthpackHit);

	mp_Player->Magnet(mp_Keys, m_SpeedAdjust);
	int keyHit = mp_Player->CollectItem(mp_Keys, Collectable::KEY);
	if (keyHit >= 0) mp_Keys.erase(std::begin(mp_Keys) + keyHit);

	mp_Player->SpikeCheck(mp_Spikes);

	//mp_PLight->SetPosition({ mp_Player->GetX(), mp_Player->GetY(), mp_Player->GetZ() });
}

void Game::MoveBullets()
{
	int bulletCounter = 0;
	while (bulletCounter != mp_Bullets.size())
	{
		int targetHit{ -1 };

		// only sweep for enemies if not in debug as game will be running faster
		bool destroyBullet = mp_Bullets[bulletCounter]->Move(mp_Blockers, mp_Enemies, &targetHit, m_SpeedAdjust); // , !m_DebugMode); for additional optimisation
		if (targetHit >= 0)
		{
			mp_Player->ShowHit();
			if (mp_Enemies[targetHit]->IsDead())
			{
				int k = mp_Keys.size();
				mp_Keys.push_back(mp_Enemies[targetHit]->SpawnKey(!m_DebugMode));
				mp_Enemies.erase(std::begin(mp_Enemies) + targetHit); //Delete enemy the bullet has hit
			}
		}
		if (destroyBullet) mp_Bullets.erase(std::begin(mp_Bullets) + bulletCounter); //Delete bullet registering collision
		else ++bulletCounter; //Allow bullets to be deleted mid-loop without out-indexing vector
	}
}

void Game::ScoreSpikes()
{
	int spikeScore = 0;
	for (GameObject* s : mp_Spikes)
	{
		for (GameObject* m : mp_Movables)
		{
			if (s->CheckCollision(m))
			{
				spikeScore++;
				s->SetParticles(false, true);
				break;
			}
			else s->SetParticles(true);
		}
		s->Fall(mp_Obstacles, m_FloorHeight - m_BlockSize, m_Gravity, m_SpeedAdjust);
	}
	mp_Player->SetSpikeScore(spikeScore, mp_Spikes.size());
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
	if (mp_Window->Resized()) mp_Timer->TickFPS();
	m_FrameGap = mp_Timer->Gap();
	m_SpeedAdjust = mp_Timer->TickFPS() * m_BlockSize;
	mp_Input->ReadInputStates();
	mp_Window->ResetContext();

	if (!InGame())
	{
		if (mp_Input->KeyPressed(KEYS::ESC) || mp_Input->PadButtonPressed(PAD::BACK)) mp_Window->Destroy();
		else if (mp_Input->AnyPressed()) CreateLevel();
	}

	switch(m_State)
	{
	case GameStates::PLAY:
		mp_DLight->Rotate({ 0, m_SpeedAdjust / m_BlockSize, 0 });

		if (mp_Timer->GetTimer("Spawn") >= m_EnemyInterval)	SpawnEnemies();
		MovePlayer();
		MoveBullets();
		for (GameObject* m : mp_Movables)
		{
			m->Fall(mp_Obstacles, m_FloorHeight, m_Gravity, m_SpeedAdjust);
			m->GetPushed(mp_Pushers, mp_Obstacles);
		}
		for (Enemy* e : mp_Enemies) e->Move(mp_Player, mp_Blockers, m_SpeedAdjust);
		for (GameObject* h : mp_HealthPacks) h->Fall(mp_Obstacles, m_FloorHeight, m_Gravity, m_SpeedAdjust);
		for (GameObject* k : mp_Keys) k->Fall(mp_Obstacles, m_FloorHeight - m_BlockSize, m_Gravity, m_SpeedAdjust);

		ScoreSpikes();

		XMFLOAT3 camPos = mp_Player->GetPos();
		mp_Skybox->SetPos(camPos.x, camPos.y + m_SkyOffset, camPos.z);
		Draw();
		if (mp_Player->Dead() || mp_Player->Won()) m_State = GameStates::END;
		else if (mp_Input->KeyPressed(KEYS::P) || mp_Input->PadButtonPressed(PAD::START)) m_State = GameStates::PAUSE;
		else if (mp_Input->KeyPressed(KEYS::ESC) || mp_Input->PadButtonPressed(PAD::BACK)) m_State = GameStates::MENU;
		break;

	case GameStates::PAUSE:
		mp_2DText->AddText("PAUSED", 0, 0, .15, { 0, 0, 0, 1 }, Alignment::Centre);
		mp_2DText->AddText("PRESS-P-TO-RESUME", 0, -.25, .05, { 0, 0, 0, 1 }, Alignment::Centre);
		if (mp_Input->KeyPressed(KEYS::P) || mp_Input->PadButtonPressed(PAD::START)) m_State = GameStates::PLAY;
		break;

	case GameStates::END:
		mp_2DText->AddText(mp_Player->Won() ? "VICTORY" : "DEFEAT", 0, 0, .15, {0, 0, 0, 1}, Alignment::Centre);
		mp_2DText->AddText("PRESS-ANY-KEY-TO-REPLAY", 0, -.25, .05, { 0, 0, 0, 1 }, Alignment::Centre);
		break;

	case GameStates::MENU:
		mp_2DText->AddText("AE2", 0, 0, .15, { 0, 0, 0, 1 }, Alignment::Centre);
		mp_2DText->AddText("PRESS-ANY-KEY-TO-PLAY", 0, -.25, .05, { 0, 0, 0, 1 }, Alignment::Centre);
		mp_2DText->AddText("WASD / ARROWS / R-STICK     -    MOVE", -0.95, 0.9, 0.035, { 0, 0, 0, 1 });
		mp_2DText->AddText("SPACE / PAD-A               -    JUMP", -0.95, 0.83, 0.035, { 0, 0, 0, 1 });
		mp_2DText->AddText("SHIFT / R-STICK PRESS       -    SPRINT", -0.95, 0.76, 0.035, { 0, 0, 0, 1 });
		mp_2DText->AddText("L-CLICK / R-TRIGGER         -    SHOOT", -0.95, 0.69, 0.035, { 0, 0, 0, 1 });
		mp_2DText->AddText("R-CLICK / L-TRIGGER         -    ZOOM", -0.95, 0.62, 0.035, { 0, 0, 0, 1 });
		mp_2DText->AddText("SCROLL / QE / LR-BUMPERS    -    CHANGE GUN", -0.95, 0.55, 0.035, { 0, 0, 0, 1 });
		break;
	}
	if (m_State != GameStates::PLAY)
	{
		mp_Window->Rainbow(m_SpeedAdjust / m_BlockSize);
		mp_Window->Clear();
		mp_2DText->RenderText();
		mp_Window->Present();
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
	for (GameObject* s : mp_Spikes) s->UpdateParticles(mp_Player->GetViewMatrix(), projection, mp_Player->GetPos(), m_SpeedAdjust, !m_DebugMode);
	for (GameObject* k : mp_Keys) k->UpdateParticles(mp_Player->GetViewMatrix(), projection, mp_Player->GetPos(), m_SpeedAdjust, !m_DebugMode);

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
