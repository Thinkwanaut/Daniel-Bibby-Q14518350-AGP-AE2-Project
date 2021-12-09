#pragma once
#define _XM_NO_INSTRINSICS_
#define XM_NO_ALIGNMENT
#include <DirectXMath.h>

#include "Drawable.h"
#include "Timer.h"
#include <list>

struct Particle
{
	float Grav{ 0.001f };
	XMFLOAT3 Vel{ 1.0f, 1.0f, 1.0f };
	XMFLOAT3 Pos{ 0.0f, 0.0f, 0.0f };
	XMFLOAT4 Col{ 1.0f, 1.0f, 1.0f, 1.0f };
};

const int NUM_VERTS{ 6 }, NUM_PARTICLES{ 100 };

class ParticleGenerator : public Drawable
{
private:
	ID3D11RasterizerState* mp_RasterSolid = 0;
	ID3D11RasterizerState* mp_RasterCullBack = 0;
	ID3D11Buffer* mp_VertexBuffer;

	XMFLOAT3 m_Vertices[NUM_VERTS] =
	{
		XMFLOAT3(-1.0f, -1.0f, 0.0f),
		XMFLOAT3(1.0f, 1.0f, 0.0f),
		XMFLOAT3(-1.0f, 1.0f, 0.0f),
		XMFLOAT3(-1.0f, -1.0f, 0.0f),
		XMFLOAT3(1.0f, -1.0f, 0.0f),
		XMFLOAT3(1.0f, 1.0f, 0.0f),
	};

	float m_Interval{ 0.01 };
	bool m_Active{ true };

	Timer* mp_Timer = nullptr;

	std::list<Particle*>::iterator m_PIt;
	std::list<Particle*> m_Dead;
	std::list<Particle*> m_Alive;

public:
	ParticleGenerator(ID3D11Device* device, ID3D11DeviceContext* context, AssetManager* assets, char* model, char* texture, char* shader);
	~ParticleGenerator();

	HRESULT CreateBuffer();
	HRESULT CreateParticle();

	float Random01();
	float RandomNegativeToPositive();

	void LookAt_XZ(float x, float z);
	void LookAt(float x, float y, float z);
	void MoveForward_XZ(float step);
	void MoveForward(float step);

	void InitialiseParticle();
	void UpdateParticles(float adjust = 1.0f);

	void UpdateConstantBuffer(Particle* particle, XMMATRIX view, XMMATRIX projection);

	void DrawOne(Particle* particle, XMMATRIX view, XMMATRIX projection, XMFLOAT3 cameraPos);
	void Draw(XMMATRIX view, XMMATRIX projection, XMFLOAT3 cameraPos);
};

