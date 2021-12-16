#include "Skybox.h"

Skybox::Skybox(ID3D11Device* device, ID3D11DeviceContext* context, AssetManager* assets, char* texture, char* shader)
	: Drawable( device, context, assets, texture, shader )
{
	CreateBuffer();
	InitStates();

	SetTint({ 0.2f, 0.2f, 0.5f, 1.0f });
}

Skybox::~Skybox()
{
	if (mp_DepthWriteSolid) mp_DepthWriteSolid->Release();
	if (mp_DepthWriteSkybox) mp_DepthWriteSkybox->Release();
	if (mp_RasterSolid) mp_RasterSolid->Release();
	if (mp_RasterCullBack) mp_RasterCullBack->Release();
	if (mp_VertexBuffer) mp_VertexBuffer->Release();
}

HRESULT Skybox::CreateBuffer()
{
	D3D11_BUFFER_DESC bufferDesc;
	ZeroMemory(&bufferDesc, sizeof(bufferDesc));
	bufferDesc.Usage = D3D11_USAGE_DYNAMIC;					
	bufferDesc.ByteWidth = sizeof(m_Vertices[0]) * CUBE_VERTS;
	bufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;						
	bufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	HRESULT hr = mp_D3DDevice->CreateBuffer(&bufferDesc, NULL, &mp_VertexBuffer);

	if (FAILED(hr)) return hr;

	D3D11_MAPPED_SUBRESOURCE ms;
	mp_ImmediateContext->Map(mp_VertexBuffer, NULL, D3D11_MAP_WRITE_DISCARD, NULL, &ms);
	memcpy(ms.pData, m_Vertices, sizeof(m_Vertices[0]) * CUBE_VERTS);
	mp_ImmediateContext->Unmap(mp_VertexBuffer, NULL);						

	return S_OK;
}

HRESULT Skybox::InitStates()
{
	D3D11_RASTERIZER_DESC rDesc;
	ZeroMemory(&rDesc, sizeof(rDesc));

	rDesc.FillMode = D3D11_FILL_SOLID;
	rDesc.CullMode = D3D11_CULL_BACK;
	HRESULT hr = mp_D3DDevice->CreateRasterizerState(&rDesc, &mp_RasterSolid);
	rDesc.CullMode = D3D11_CULL_FRONT;
	hr = mp_D3DDevice->CreateRasterizerState(&rDesc, &mp_RasterCullBack);

	D3D11_DEPTH_STENCIL_DESC dsDecsc;
	ZeroMemory(&dsDecsc, sizeof(dsDecsc));
	dsDecsc.DepthEnable = true;
	dsDecsc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	dsDecsc.DepthFunc = D3D11_COMPARISON_LESS;
	hr = mp_D3DDevice->CreateDepthStencilState(&dsDecsc, &mp_DepthWriteSolid);
	dsDecsc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;
	hr = mp_D3DDevice->CreateDepthStencilState(&dsDecsc, &mp_DepthWriteSkybox);

	return S_OK;
}

void Skybox::UpdateConstantBuffer(XMMATRIX view, XMMATRIX projection, Light* ambient)
{
	XMMATRIX world = GetWorldMatrix();
	XMMATRIX transpose = XMMatrixTranspose(world);
	SKYBOX_BUFFER cb{};

	cb.WorldViewProjection = world * view * projection;
	cb.light = (ambient) ? ambient->Colour() : XMVECTOR({ 0.5f, 0.5f, 0.5f, 1 });
	cb.colour = GetTint();
	mp_ImmediateContext->UpdateSubresource(mp_ConstantBuffer, 0, 0, &cb, 0, 0);
}

void Skybox::Draw(XMMATRIX view, XMMATRIX projection, Light* ambient)
{
	SetContext();
	UpdateConstantBuffer(view, projection, ambient);

	UINT stride = sizeof(VERTEX_INFO);
	UINT offset = 0;
	mp_ImmediateContext->IASetVertexBuffers(0, 1, &mp_VertexBuffer, &stride, &offset);
	mp_ImmediateContext->RSSetState(mp_RasterCullBack);
	mp_ImmediateContext->OMSetDepthStencilState(mp_DepthWriteSkybox, NULL);
	mp_ImmediateContext->Draw(CUBE_VERTS, 0);
	mp_ImmediateContext->RSSetState(mp_RasterSolid);
	mp_ImmediateContext->OMSetDepthStencilState(mp_DepthWriteSolid, NULL);
}
