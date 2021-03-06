#pragma once
#include "Window.h"

#define _XM_NO_INSTRINSICS_
#define XM_NO_ALIGNMENT
#include <DirectXMath.h>
#include <fstream>

#include "Input.h"
#include "Player.h"
#include "text2D.h"
#include "DirectionalLight.h"
#include "PointLight.h"
#include "Skybox.h"
#include "Enemy.h"
#include "Bullet.h"
#include "AssetManager.h"
#include "Timer.h"

using namespace DirectX;
enum class ObjectTypes
{
	WALL = 'W',
	MOVABLE = 'M',
	SPIKES = 'S',
	HEALTH = 'H',
	ENEMY = 'E',
	PLAYER = 'P',
};

enum class GameStates
{
	MENU,
	PLAY,
	PAUSE,
	END
};

const XMFLOAT4 RED{ 1.0f, 0.0f, 0.0f, 1.0f };
const XMFLOAT4 YELLOW{ 1.0f, 1.0f, 0.0f, 1.0f };
const XMFLOAT4 GREEN{ 0.0f, 1.0f, 0.0f, 1.0f };
const XMFLOAT4 CYAN{ 0.0f, 1.0f, 1.0f, 1.0f };
const XMFLOAT4 BLUE{ 0.0f, 0.0f, 1.0f, 1.0f };
const XMFLOAT4 MAGENTA{ 1.0f, 0.0f, 1.0f, 1.0f };

class Game
{
private:
	char* CUBE_FILE = (char*)"Assets/Cube.obj";
	char* SPHERE_FILE = (char*)"Assets/Sphere.obj";
	char* BOX_FILE = (char*)"Assets/BoxTexture.bmp";
	char* STONE_FILE = (char*)"Assets/StoneTexture.bmp";
	char* FLOOR_FILE = (char*)"Assets/FloorTexture.bmp";
	char* FLOOR_COLUMN_FILE = (char*)"Assets/FloorTextureTall.bmp";
	char* FLOOR_ROW_FILE = (char*)"Assets/FloorTextureWide.bmp";
	char* FENCE_FILE = (char*)"Assets/Fence2.png";
	char* SPIKE_FILE = (char*)"Assets/SpikeTexture.png";
	char* ENEMY_FILE = (char*)"Assets/EnemyTexture.bmp";
	char* HEALTH_FILE = (char*)"Assets/HealthTexture.bmp";
	char* ORB_FILE = (char*)"Assets/OrbTexture.bmp";
	char* BLANK_FILE = (char*)"Assets/BlankTexture.bmp";
	char* SKY_FILE = (char*)"Assets/skybox02.dds";
	char* SHADER_FILE = (char*)"shaders.hlsl";
	char* SKY_SHADER_FILE = (char*)"skyShaders.hlsl";
	char* LEVEL_FILE = (char*)"Assets/Level.txt";

	Window* mp_Window = nullptr;

	Input* mp_Input = nullptr;
	AssetManager* mp_Assets = nullptr;

	Light* mp_ALight = new Light({ 0.5f, 0.5f, 0.5f, 1.0f });
	DirectionalLight* mp_DLight = new DirectionalLight({ 0, 0, -1 }, { 0.5, 0.5, 0.5, 1 });
	PointLight* mp_PLight = new PointLight({ 0, 0, 5 }, { 500, 0, 0, 1 });
	Player* mp_Player = nullptr;
	Text2D* mp_2DText = nullptr;	
	Timer* mp_Timer = nullptr;

	std::vector<string> m_Layout;

	std::vector<GameObject*> mp_Obstacles, mp_Movables, mp_Pushers, mp_Spikes, mp_HealthPacks, mp_Keys, mp_Spawns, mp_Blockers;
	std::vector<Enemy*> mp_Enemies;
	std::vector<Bullet*> mp_Bullets;

	std::map<string, Particle> m_ParticleTemplates;

	Skybox* mp_Skybox = nullptr;

	float m_SpeedAdjust{ 1.0f }, m_FrameGap{ 0.0f };

	float m_BlockSize{ 10 }, m_FloorHeight{ -10 }, m_ObjectHeight{ 0 }, m_Gravity{ 0.001f }, m_SkyOffset{ 1 }, m_EnemyInterval{ 5.0f };

	bool m_SingleFloor{ false }, m_DebugMode{ false };

	GameStates m_State;

public:
	Game(_In_ HINSTANCE hInstance, _In_ int nCmdShow, Window* window);
	~Game();

	void DestroyLevel();

	void SetLayout();

	void CreateLevel();

	int Run();
	bool InGame();

	void SpawnEnemies();
	void MovePlayer();
	void MoveBullets();
	void ScoreSpikes();
	XMMATRIX XYZRotation(float x, float y, float z);

	void Update();
	void Draw();

	XMFLOAT2 PixelToVertex(XMFLOAT2 pixelCoords);
	XMFLOAT2 VertexToPixel(XMFLOAT2 vertexCoords);
};

