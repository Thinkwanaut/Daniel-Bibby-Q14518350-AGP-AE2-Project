#include "Drawable.h"

Drawable::Drawable(ID3D11Device* device, ID3D11DeviceContext* context, AssetManager* assets, char* texture, char* shader)
	: mp_D3DDevice{ device }, mp_ImmediateContext{ context }, mp_Assets{ assets }
{
	mp_VShader = assets->GetVShader(shader);
	mp_PShader = assets->GetPShader(shader);
	mp_InputLayout = assets->GetInputLayout(shader);
	mp_ConstantBuffer = assets->GetBuffer(shader);
	mp_Sampler = assets->GetSampler(shader);
	mp_Texture = assets->GetTexture(texture);
}

void Drawable::SetPos(float x, float y, float z)
{
	m_x = x; m_y = y; m_z = z;
}

void Drawable::SetRot(float x, float y, float z)
{
	m_xRot = x; m_yRot = y; m_zRot = z;
}

void Drawable::SetScale(float x, float y, float z)
{
	m_xScale = x; m_yScale = y; m_zScale = z;
}

void Drawable::SetPos(XMFLOAT3 pos)
{
	m_x = pos.x; m_y = pos.y; m_z = pos.z;
}

void Drawable::SetRot(XMFLOAT3 rot)
{
	m_xRot = rot.x;	m_yRot = rot.y;	m_zRot = rot.z;
}

void Drawable::SetScale(XMFLOAT3 scale)
{
	m_xScale = scale.x;	m_yScale = scale.y; m_zScale = scale.z;
}

void Drawable::SetColour(XMVECTOR colour)
{
	m_baseColour = colour;
}

XMMATRIX Drawable::GetWorldMatrix()
{
	XMMATRIX scale, rotation, translation;
	scale = XMMatrixScaling(m_xScale, m_yScale, m_zScale);
	rotation = XYZRotation(m_xRot, m_yRot, m_zRot);
	translation = XMMatrixTranslation(m_x, m_y, m_z);

	return scale * rotation * translation;
}

XMMATRIX Drawable::XYZRotation(float x, float y, float z)
{
	XMMATRIX xMatrix, yMatrix, zMatrix;
	xMatrix = XMMatrixRotationX(XMConvertToRadians(x));
	yMatrix = XMMatrixRotationY(XMConvertToRadians(y));
	zMatrix = XMMatrixRotationZ(XMConvertToRadians(z));
	return xMatrix * yMatrix * zMatrix;
}

XMFLOAT3 Drawable::GetPos()
{
	return { m_x, m_y, m_z };
}

XMFLOAT3 Drawable::GetRot()
{
	return { m_xRot, m_yRot, m_zRot };
}

XMFLOAT3 Drawable::GetScale()
{
	return { m_xScale, m_yScale, m_zScale };
}

void Drawable::SetX(float x)
{
	m_x = x;
}

void Drawable::SetY(float y)
{
	m_y = y;
}

void Drawable::SetZ(float z)
{
	m_z = z;
}

void Drawable::SetContext()
{
	mp_ImmediateContext->VSSetConstantBuffers(0, 1, &mp_ConstantBuffer);
	mp_ImmediateContext->VSSetShader(mp_VShader, 0, 0);
	mp_ImmediateContext->PSSetShader(mp_PShader, 0, 0);
	mp_ImmediateContext->IASetInputLayout(mp_InputLayout);
	mp_ImmediateContext->PSSetShaderResources(0, 1, &mp_Texture);
	mp_ImmediateContext->PSSetSamplers(0, 1, &mp_Sampler);
}

void Drawable::Draw(XMMATRIX view, XMMATRIX projection, Light* ambient, DirectionalLight* directional)
{
	SetContext();

	XMMATRIX world = GetWorldMatrix();
	XMMATRIX transpose = XMMatrixTranspose(world);
	DRAW_BUFFER cb{};

	cb.WorldViewProjection = world * view * projection;
	cb.base_colour = m_baseColour;
	cb.ambient_light_colour = (ambient) ? ambient->Colour() : XMVECTOR({0.5f, 0.5f, 0.5f, 1});
	cb.directional_light_colour = (directional) ? directional->Colour() : XMVECTOR({ 0, 0, 0, 1 });
	cb.directional_light_vector = (directional) ? XMVector3Transform(directional->Direction(), transpose) : XMVECTOR({ 0, 0, 0 });
	cb.directional_light_vector = XMVector3Normalize(cb.directional_light_vector);

	mp_ImmediateContext->UpdateSubresource(mp_ConstantBuffer, 0, 0, &cb, 0, 0);
}
