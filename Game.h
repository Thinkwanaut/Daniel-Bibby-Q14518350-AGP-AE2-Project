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

class Game
{
private:
	char* CUBE_FILE = (char*)"Assets/Cube.obj";
	char* SPHERE_FILE = (char*)"Assets/Sphere.obj";
	char* BOX_FILE = (char*)"Assets/BoxTexture.bmp";
	char* FENCE_FILE = (char*)"Assets/fence.png";
	char* ENEMY_FILE = (char*)"Assets/EnemyTexture.bmp";
	char* HEALTH_FILE = (char*)"Assets/HealthTexture.bmp";
	char* ORB_FILE = (char*)"Assets/OrbTexture.bmp";
	char* SKY_FILE = (char*)"Assets/skybox02.dds";
	char* SHADER_FILE = (char*)"shaders.hlsl";
	char* LEVEL_FILE = (char*)"Assets/Level.txt";

	Window* mp_Window = nullptr;

	Input* mp_Input = nullptr;
	AssetManager* mp_Assets = nullptr;

	Light* mp_ALight = new Light({ 0.5f, 0.5f, 0.5f, 1.0f });
	DirectionalLight* mp_DLight = new DirectionalLight({ 0, 0, -1 }, { 0, 0, 0, 1 });
	PointLight* mp_PLight = new PointLight({ 0, 0, 5 }, { 500, 0, 0, 1 });
	Player* mp_Player = nullptr;
	Text2D* mp_2DText = nullptr;	
	Timer* mp_Timer = nullptr;

	std::vector<string> m_Layout;

	std::vector<GameObject*> mp_Obstacles, mp_Movables, mp_Pushers, mp_Spikes, mp_HealthPacks, mp_Keys;
	std::vector<Enemy*> mp_Enemies;
	std::vector<Bullet*> mp_Bullets;

	Skybox* mp_Skybox = nullptr;

	float m_BlockSize{ 1 }, m_FloorHeight{ -5 }, m_ObjectHeight{ 0 }, m_Gravity{ 0.01f }, m_SkyOffset{ 1 };

	bool m_SingleFloor{ false }, m_Paused{ false }, m_GameEnded{ false };

	int m_MaxScore{ 3 };

	string m_PauseText{ "PAUSED" };

public:
	Game(_In_ HINSTANCE hInstance, _In_ int nCmdShow, Window* window);
	~Game();

	void DestroyLevel();

	void SetLayout();

	void CreateLevel();

	int Run();

	void MovePlayer(float adjust = 1);
	void MoveBullets(float fpsAdjustment = 1);
	XMMATRIX XYZRotation(float x, float y, float z);

	void Update();
	void Draw();

	XMFLOAT2 PixelToVertex(XMFLOAT2 pixelCoords);
	XMFLOAT2 VertexToPixel(XMFLOAT2 vertexCoords);
};

