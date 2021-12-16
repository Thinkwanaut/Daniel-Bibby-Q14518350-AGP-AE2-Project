#include "ParticleGenerator.h"

ParticleGenerator::ParticleGenerator(ID3D11Device* device, ID3D11DeviceContext* context, AssetManager* assets, char* texture, char* shader)
	: Drawable{ device, context, assets, texture, shader }
{
	CreateParticle();
	mp_Timer = new Timer();
	mp_Timer->StartTimer("Particle");

	for (int p = 0; p < m_ParticleNum; p++) mp_Dead.push_back(new Particle());
}

ParticleGenerator::~ParticleGenerator()
{
	ClearParticles();

	if (mp_RasterSolid) mp_RasterSolid->Release();
	if (mp_RasterCullBack) mp_RasterCullBack->Release();
	if (mp_VertexBuffer) mp_VertexBuffer->Release();

	if (mp_Timer)
	{
		delete mp_Timer;
		mp_Timer = nullptr;
	}
}

HRESULT ParticleGenerator::CreateParticle()
{
	HRESULT hr = S_OK;

	D3D11_RASTERIZER_DESC rDesc;
	ZeroMemory(&rDesc, sizeof(rDesc));

	rDesc.FillMode = D3D11_FILL_SOLID;
	rDesc.CullMode = D3D11_CULL_NONE;
	hr = mp_D3DDevice->CreateRasterizerState(&rDesc, &mp_RasterSolid);
	rDesc.CullMode = D3D11_CULL_BACK;
	hr = mp_D3DDevice->CreateRasterizerState(&rDesc, &mp_RasterCullBack);

	D3D11_BUFFER_DESC bufferDesc;
	ZeroMemory(&bufferDesc, sizeof(bufferDesc));
	bufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	bufferDesc.ByteWidth = sizeof(m_Vertices[0]) * 6;
	bufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	hr = mp_D3DDevice->CreateBuffer(&bufferDesc, NULL, &mp_VertexBuffer);

	if (FAILED(hr)) return hr;

	D3D11_MAPPED_SUBRESOURCE ms;
	mp_ImmediateContext->Map(mp_VertexBuffer, NULL, D3D11_MAP_WRITE_DISCARD, NULL, &ms);
	memcpy(ms.pData, m_Vertices, sizeof(m_Vertices[0]) * 6);
	mp_ImmediateContext->Unmap(mp_VertexBuffer, NULL);

	return S_OK;
}

void ParticleGenerator::ClearParticles()
{
	for (Particle* dp : mp_Dead) delete dp;
	mp_Dead.clear();

	for (Particle* ap : mp_Alive) delete ap;
	mp_Alive.clear();
}

void ParticleGenerator::SetPreset(Presets type)
{
	m_Colours.clear();
	switch (type)
	{
	case Presets::FIRE:
		AddColour({ 1.f, 1.f, 0.f, 1.0f });
		AddColour({ 1.0f, .0f, 0.0f, 1.0f });
		AddColour({ 0.0f, 0.0f, 0.0f, 0.0f });
		SetSize({ 0.1f, 2.0f, 0.1f });
		SetVelocity({ 0.0f, 0.02f, 0.0f }, 0.0f);
		SetNumber(200);
		SetInterval(0.005f);
		SetLifetime(1.f);
		SetGravity(0.0f);
		break;

	case Presets::KEY:
		AddColour({ 1.f, 1.f, 1.f, 1.0f });
		AddColour({ 1.0f, 1.0f, 1.0f, 0.0f });
		SetSize({ 0.25f, 0.25f, 0.25f });
		SetVelocity({ 0.0f, 0.0f, 0.0f }, 0.025f);
		SetAcceleration({ 0.9975f, 0.9975f, 0.9975f });
		SetNumber(50);
		SetInterval(0.02f);
		SetLifetime(1.f);
		SetGravity(0.0f);
		SetRandSpawn(false, false, false);
		break;
	default:
		break;
	}
}

void ParticleGenerator::SetActive(bool active, bool clearActive)
{
	m_Active = active;
	if (!active && clearActive)
	{
		m_PIt = mp_Alive.begin();
		while (m_PIt != mp_Alive.end())
		{
			mp_Dead.push_back(mp_Alive.front());
			m_PIt = mp_Alive.erase(mp_Alive.begin());
		}
	}
}

void ParticleGenerator::SetNumber(int num)
{
	if (num > m_ParticleNum) 
		for (int p = m_ParticleNum; p < num; p++) mp_Dead.push_back(new Particle());
	else
	{
		ClearParticles();
		for (int p = 0; p < num; p++) mp_Dead.push_back(new Particle());
	}
	m_ParticleNum = num;
}

void ParticleGenerator::SetLifetime(float life)
{
	m_LifeTime = life;
}

void ParticleGenerator::StartTimer()
{
	mp_Timer->StartTimer("Particle");
}

void ParticleGenerator::SetSize(float x, float y, float z, float rand)
{
	m_Size = { x, y, z };
	if (rand >= 0.0f) m_SizeRand = rand;
}

void ParticleGenerator::SetSize(XMFLOAT3 size, float rand)
{
	m_Size = size;
	if (rand >= 0.0f) m_SizeRand = rand;
}

void ParticleGenerator::AddColour(float r, float g, float b, float a)
{
	m_Colours.push_back({ r, g, b, a });
}

void ParticleGenerator::AddColour(XMFLOAT4 newCol)
{
	m_Colours.push_back(newCol);
}

void ParticleGenerator::SetColour(float r, float g, float b, float a, int index)
{
	if (index >= m_Colours.size()) return;

	m_Colours[index] = { r, g, b, a };
}

void ParticleGenerator::SetColour(XMFLOAT4 col, int index)
{
	if (index >= m_Colours.size()) return;

	m_Colours[index] = col;
}

void ParticleGenerator::SetVelocity(float x, float y, float z, float rand)
{
	m_BaseVelocity = { x, y, z };
	if (rand >= 0.0f) m_VelRand = rand;
}

void ParticleGenerator::SetVelocity(XMFLOAT3 vel, float rand)
{
	m_BaseVelocity = vel;
	if (rand >= 0.0f) m_VelRand = rand;
}

void ParticleGenerator::SetInterval(float interval)
{
	m_Interval = interval;
}

void ParticleGenerator::SetAcceleration(float x, float y, float z)
{
	m_Acceleration = { x, y, z };
}

void ParticleGenerator::SetAcceleration(XMFLOAT3 vel)
{
	m_Acceleration = vel;
}

void ParticleGenerator::SetGravity(float grav)
{
	m_Grav = grav;
}

void ParticleGenerator::SetRandSpawn(bool x, bool y, bool z)
{
	m_RandSpawn[0] = x;
	m_RandSpawn[1] = y;
	m_RandSpawn[2] = z;
}

void ParticleGenerator::SpawnParticle()
{
	if (mp_Dead.size() == 0 || !m_Active) return;

	m_PIt = mp_Dead.begin();

	(*m_PIt)->Pos = { m_x, m_y, m_z };
	if (m_RandSpawn[0]) (*m_PIt)->Pos.x += RandomPN(m_xScale);
	if (m_RandSpawn[1]) (*m_PIt)->Pos.y += RandomPN(m_yScale);
	if (m_RandSpawn[2]) (*m_PIt)->Pos.z += RandomPN(m_zScale);

	XMFLOAT3 velRand = Random3(0, m_VelRand);
	(*m_PIt)->Vel = { m_BaseVelocity.x + velRand.x, m_BaseVelocity.y + velRand.y , m_BaseVelocity.z + velRand.z };
	float sizeRand = RandomMultiplier(m_SizeRand);
	(*m_PIt)->Size = { m_Size.x * sizeRand, m_Size.y * sizeRand, m_Size.z * sizeRand };

	for (int c = 0; c < m_Colours.size(); c++)
	{
		if (c >= (*m_PIt)->Col.size()) (*m_PIt)->Col.push_back({ 0.0f, 0.0f, 0.0f, 0.0f });
		(*m_PIt)->Col[c] = { m_Colours[c].x * RandomMultiplier(m_ColRand),
			m_Colours[c].y * RandomMultiplier(m_ColRand),
			m_Colours[c].z * RandomMultiplier(m_ColRand),
			m_Colours[c].w };
	}
	
	// When all particles have been spawned, reuse particle indices to avoid creating new timers
	if ((*m_PIt)->ID == -1) (*m_PIt)->ID = m_SpawnIndex++;
	mp_Timer->StartTimer(std::to_string((*m_PIt)->ID));

	mp_Alive.push_back(*m_PIt);
	mp_Dead.pop_front();
}

void ParticleGenerator::UpdateParticles(float adjust, bool catchUp)
{
	if (mp_Timer->GetTimer("Particle") > m_Interval)
	{
		for (int p = 0; p < (catchUp ? floorf(mp_Timer->GetTimer("Particle") / m_Interval) : 1); p++) SpawnParticle();
		mp_Timer->StartTimer("Particle");
	}

	if (mp_Alive.size() == NULL) return;

	m_PIt = mp_Alive.begin();
	while (m_PIt != mp_Alive.end())
	{
		(*m_PIt)->Vel.y -= m_Grav;

		(*m_PIt)->Vel.x *= powf(m_Acceleration.x, adjust);
		(*m_PIt)->Vel.y *= powf(m_Acceleration.y, adjust);
		(*m_PIt)->Vel.z *= powf(m_Acceleration.z, adjust);
		
		(*m_PIt)->Pos.x += (*m_PIt)->Vel.x * adjust;
		(*m_PIt)->Pos.y += (*m_PIt)->Vel.y * adjust;
		(*m_PIt)->Pos.z += (*m_PIt)->Vel.z * adjust;

		if (mp_Timer->GetTimer(std::to_string((*m_PIt)->ID)) > m_LifeTime)
		{
			mp_Dead.push_back(mp_Alive.front());
			m_PIt = mp_Alive.erase(mp_Alive.begin());
		}

		else m_PIt++;
	}
}

void ParticleGenerator::UpdateConstantBuffer(Particle* particle, XMMATRIX view, XMMATRIX projection, XMFLOAT3 cameraPos)
{
	/*float lookAngle = XMVectorGetX(XMVector2AngleBetweenVectors(
		{ cameraPos.x - particle->Pos.x, cameraPos.z - particle->Pos.z },
		{ particle->Vel.x, particle->Vel.z }));
	XMFLOAT2 perpVel = Rotate2({ particle->Vel.x, particle->Vel.z }, -(XM_PI / 2.0f - lookAngle));
	float perpVelM = powf(powf(perpVel.x, 2) + powf(perpVel.y, 2), 0.5f);
	float zrot = atan2(perpVelM, particle->Vel.y);*/ // An attempt at rotating particle based on velocity relative to the player to create an arc.

	float yrot = atan2f(cameraPos.x - particle->Pos.x, cameraPos.z - particle->Pos.z);
	float xrot = -atan2f(cameraPos.y - particle->Pos.y, powf(powf(cameraPos.x - particle->Pos.x, 2) + powf(cameraPos.z - particle->Pos.z, 2), 0.5f));

	XMMATRIX world = XMMatrixScaling(particle->Size.x, particle->Size.y, particle->Size.z);
	world *= XMMatrixRotationX(xrot) * XMMatrixRotationY(yrot);// *XMMatrixRotationZ(zrot);
	world *= XMMatrixTranslation(particle->Pos.x, particle->Pos.y, particle->Pos.z);

	PARTICLE_BUFFER cb{};
	cb.WorldViewProjection = world * view * projection;
	int currentColour = floorf(min(mp_Timer->GetTimer(std::to_string(particle->ID)) / m_LifeTime, 1) * (m_Colours.size() - 1));
	if (currentColour + 1 < particle->Col.size())
	{
		float colourTimer = fmodf(mp_Timer->GetTimer(std::to_string(particle->ID)) / m_LifeTime * (m_Colours.size() - 1), 1.0f);
		cb.colour = Lerp4(particle->Col[currentColour], particle->Col[currentColour + 1], colourTimer);
	}
	else cb.colour = particle->Col[currentColour];


	mp_ImmediateContext->UpdateSubresource(mp_ConstantBuffer, 0, 0, &cb, 0, 0);
}

void ParticleGenerator::DrawOne(Particle* particle, XMMATRIX view, XMMATRIX projection, XMFLOAT3 cameraPos)
{
	UpdateConstantBuffer(particle, view, projection, cameraPos);
	SetContext();
	mp_ImmediateContext->Draw(PARTICLE_VERTS, 0);
}

void ParticleGenerator::Draw(XMMATRIX view, XMMATRIX projection, XMFLOAT3 cameraPos)
{
	UINT stride = sizeof(XMFLOAT3), offset = 0;
	mp_ImmediateContext->IASetVertexBuffers(0, 1, &mp_VertexBuffer, &stride, &offset);
	mp_ImmediateContext->RSSetState(mp_RasterCullBack); 
	for (Particle* p : mp_Alive) DrawOne(p, view, projection, cameraPos);
	mp_ImmediateContext->RSSetState(mp_RasterSolid);
}
