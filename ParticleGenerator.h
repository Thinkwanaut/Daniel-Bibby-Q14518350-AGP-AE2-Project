#pragma once
#define _XM_NO_INSTRINSICS_
#define XM_NO_ALIGNMENT
#include <DirectXMath.h>
#include <list>

#include "Drawable.h"
#include "Timer.h"
#include "MoreMaths.h"

struct Particle
{
	XMFLOAT3 Vel{ 1.0f, 1.0f, 1.0f };
	XMFLOAT3 Pos{ 0.0f, 0.0f, 0.0f };
	XMFLOAT3 Size{ 1.0f, 1.0f, 1.0f };
	std::vector<XMFLOAT4> Col{ { 1.0f, 1.0f, 0.0f, 1.0f } };
	int ID{ -1 };
};

const int PARTICLE_VERTS{ 6 };

class ParticleGenerator : public Drawable
{
private:
	ID3D11RasterizerState* mp_RasterSolid = 0;
	ID3D11RasterizerState* mp_RasterCullBack = 0;
	ID3D11Buffer* mp_VertexBuffer;

	XMFLOAT3 m_Vertices[PARTICLE_VERTS] =
	{
		XMFLOAT3(-1.0f, -1.0f, 0.0f),
		XMFLOAT3(1.0f, 1.0f, 0.0f),
		XMFLOAT3(-1.0f, 1.0f, 0.0f),
		XMFLOAT3(-1.0f, -1.0f, 0.0f),
		XMFLOAT3(1.0f, -1.0f, 0.0f),
		XMFLOAT3(1.0f, 1.0f, 0.0f),
	};

	int m_ParticleNum{ 100 };

	int m_SpawnIndex{ 0 };
	float m_Interval{ 0.01 }, m_LifeTime{ 1.0f }, m_Grav{ 0.001f };
	bool m_Active{ true };

	Timer* mp_Timer = nullptr;

	std::list<Particle*>::iterator m_PIt;
	std::list<Particle*> mp_Dead;
	std::list<Particle*> mp_Alive;

	std::vector<XMFLOAT4> m_Colours = { {1.0f, 1.0f, 1.0f, 1.0f} };
	XMFLOAT3 m_Size{ 0.1f, 0.1f, 0.1f };
	XMFLOAT3 m_BaseVelocity{ 0.0f, 0.1f, 0.0f };
	XMFLOAT3 m_Acceleration{ .999f, .999f, .999f }; // *

	float m_ColRand{ 1.f }; // *
	float m_SizeRand{ .0f }; // *
	float m_VelRand{ 0.1f }; // +

public:
	ParticleGenerator(ID3D11Device* device, ID3D11DeviceContext* context, AssetManager* assets, char* texture, char* shader);
	~ParticleGenerator();

	HRESULT CreateParticle();
	void ClearParticles();

	void SetActive(bool active, bool clearActive = false);

	void SetNumber(int num);
	void SetLifetime(float life);
	void SetSize(float x, float y, float z, float rand = 0.0f);
	void SetSize(XMFLOAT3 size, float rand = 0.0f);
	void AddColour(float r, float g, float b, float a);
	void AddColour(XMFLOAT4 newCol);
	void SetColour(float r, float g, float b, float a, int index = 0);
	void SetColour(XMFLOAT4 col, int index = 0);
	void SetVelocity(float x, float y, float z, float rand = 0.0f);
	void SetVelocity(XMFLOAT3 vel, float rand = 0.0f);
	void SetAcceleration(float x, float y, float z);
	void SetAcceleration(XMFLOAT3 vel);
	void SetGravity(float grav);

	void SpawnParticle();
	void UpdateParticles(float adjust = 1.0f);

	void UpdateConstantBuffer(Particle* particle, XMMATRIX view, XMMATRIX projection, XMFLOAT3 cameraPos);

	void DrawOne(Particle* particle, XMMATRIX view, XMMATRIX projection, XMFLOAT3 cameraPos);
	void Draw(XMMATRIX view, XMMATRIX projection, XMFLOAT3 cameraPos);
};

