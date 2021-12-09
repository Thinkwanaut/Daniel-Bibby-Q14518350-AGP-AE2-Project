#include "ParticleGenerator.h"

ParticleGenerator::ParticleGenerator(ID3D11Device* device, ID3D11DeviceContext* context, AssetManager* assets, char* model, char* texture, char* shader)
	: Drawable{ device, context, assets, texture, shader }
{
	CreateBuffer();
	CreateParticle();
	mp_Timer = new Timer();
	mp_Timer->StartTimer("Particle");
}

ParticleGenerator::~ParticleGenerator()
{
	if (mp_RasterSolid) mp_RasterSolid->Release();
	if (mp_RasterCullBack) mp_RasterCullBack->Release();
	if (mp_VertexBuffer) mp_VertexBuffer->Release();
}

float ParticleGenerator::Random01()
{
	return (float)rand() / (float)RAND_MAX;
}

float ParticleGenerator::RandomNegativeToPositive()
{
	return (Random01() - 0.5f) * 2.0f;
}

HRESULT ParticleGenerator::CreateBuffer()
{
	D3D11_BUFFER_DESC bufferDesc;
	ZeroMemory(&bufferDesc, sizeof(bufferDesc));
	bufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	bufferDesc.ByteWidth = sizeof(m_Vertices[0]) * NUM_VERTS;
	bufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	HRESULT hr = mp_D3DDevice->CreateBuffer(&bufferDesc, NULL, &mp_VertexBuffer);

	if (FAILED(hr)) return hr;

	D3D11_MAPPED_SUBRESOURCE ms;
	mp_ImmediateContext->Map(mp_VertexBuffer, NULL, D3D11_MAP_WRITE_DISCARD, NULL, &ms);
	memcpy(ms.pData, m_Vertices, sizeof(m_Vertices[0]) * NUM_VERTS);
	mp_ImmediateContext->Unmap(mp_VertexBuffer, NULL);

	return S_OK;
}

HRESULT ParticleGenerator::CreateParticle()
{
	HRESULT hr = S_OK;

	D3D11_RASTERIZER_DESC rDesc;
	ZeroMemory(&rDesc, sizeof(rDesc));

	rDesc.FillMode = D3D11_FILL_SOLID;
	rDesc.CullMode = D3D11_CULL_BACK;
	hr = mp_D3DDevice->CreateRasterizerState(&rDesc, &mp_RasterSolid);
	rDesc.CullMode = D3D11_CULL_FRONT;
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

void ParticleGenerator::LookAt_XZ(float x, float z)
{
	float dx = x - m_x;
	float dz = z - m_z;
	m_yRot = XMConvertToDegrees(atan2f(dx, dz));
}

void ParticleGenerator::LookAt(float x, float y, float z)
{
	LookAt_XZ(x, z);
	m_xRot = XMConvertToDegrees(-atan2f(y - m_y, powf(powf(x - m_x, 2) + powf(z - m_z, 2), 0.5f)));
}

void ParticleGenerator::MoveForward_XZ(float step)
{
	m_x += sin(XMConvertToRadians(m_yRot)) * step;
	m_z += cos(XMConvertToRadians(m_yRot)) * step;
}

void ParticleGenerator::MoveForward(float step)
{
	m_x += sin(XMConvertToRadians(m_yRot)) * step * cos(XMConvertToRadians(m_xRot));
	m_y += -sin(XMConvertToRadians(m_xRot)) * step;
	m_z += cos(XMConvertToRadians(m_yRot)) * step * cos(XMConvertToRadians(m_xRot));
}

void ParticleGenerator::InitialiseParticle()
{
	if (m_Dead.size() == NULL || !m_Active) return;

	m_PIt = m_Dead.begin();

	(*m_PIt)->Pos = GetPos();

	m_Alive.push_back(*m_PIt);
	m_Dead.pop_front();

	mp_Timer->StartTimer("Particle");
}

void ParticleGenerator::UpdateParticles(float adjust)
{
	if (m_Alive.size() == NULL) return;

	m_PIt = m_Alive.begin();
	while (m_PIt != m_Alive.end())
	{
		(*m_PIt)->Vel.y += (*m_PIt)->Grav * adjust;
		(*m_PIt)->Pos.x += (*m_PIt)->Vel.x * adjust;
		(*m_PIt)->Pos.y += (*m_PIt)->Vel.y * adjust;
		(*m_PIt)->Pos.z += (*m_PIt)->Vel.z * adjust;
	}
}

void ParticleGenerator::UpdateConstantBuffer(Particle* particle, XMMATRIX view, XMMATRIX projection)
{
	XMMATRIX world = GetWorldMatrix();
	XMMATRIX transpose = XMMatrixTranspose(world);
	PARTICLE_BUFFER cb{};

	cb.WorldViewProjection = world * view * projection;
	cb.colour = particle->Col;

	mp_ImmediateContext->UpdateSubresource(mp_ConstantBuffer, 0, 0, &cb, 0, 0);
}

void ParticleGenerator::DrawOne(Particle* particle, XMMATRIX view, XMMATRIX projection, XMFLOAT3 cameraPos)
{
	UpdateConstantBuffer(particle, view, projection);
	mp_ImmediateContext->Draw(NUM_VERTS, 0);
}

void ParticleGenerator::Draw(XMMATRIX view, XMMATRIX projection, XMFLOAT3 cameraPos)
{
	UINT stride = sizeof(XMFLOAT3), offset = 0;
	mp_ImmediateContext->IASetVertexBuffers(0, 1, &mp_VertexBuffer, &stride, &offset);
	mp_ImmediateContext->RSSetState(mp_RasterCullBack); 
	for (Particle* p : m_Alive) DrawOne(p, view, projection, cameraPos);
	mp_ImmediateContext->RSSetState(mp_RasterSolid);
}
