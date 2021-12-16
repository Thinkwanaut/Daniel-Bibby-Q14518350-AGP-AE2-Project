#pragma once
#include <d3d11.h>
#include <d3dx11.h>
#include <dxerr.h>
#define _XM_NO_INSTRINSICS_
#define XM_NO_ALIGNMENT
#include <DirectXMath.h>
#include "DirectionalLight.h"
#include "AssetManager.h"
#include "PointLight.h"
#include "Timer.h"
#include "MoreMaths.h"

using namespace DirectX;
class Drawable
{
protected:
	AssetManager* mp_Assets = nullptr;

	ID3D11Device* mp_D3DDevice = NULL;
	ID3D11DeviceContext* mp_ImmediateContext = NULL;

	ID3D11VertexShader* mp_VShader = NULL;
	ID3D11PixelShader* mp_PShader = NULL;
	ID3D11InputLayout* mp_InputLayout = NULL;
	ID3D11Buffer* mp_ConstantBuffer = NULL;
	ID3D11ShaderResourceView* mp_Texture = NULL;
	ID3D11SamplerState* mp_Sampler = NULL;

	float m_x{ 0 }, m_y{ 0 }, m_z{ 0 };
	float m_xRot{ 0 }, m_yRot{ 0 }, m_zRot{ 0 };
	float m_xScale{ 1 }, m_yScale{ 1 }, m_zScale{ 1 };

	std::vector<XMFLOAT4> m_Tint{ { 0, 0, 0, 1 } };
	XMFLOAT4 m_AddedColour{ 0, 0, 0, 0 };
	float m_ColourDuration{ 1.0f };

	Timer* mp_Timer = nullptr;

public:
	Drawable(ID3D11Device* device, ID3D11DeviceContext* context, AssetManager* assets, char* texture, char* shader);

	XMMATRIX GetWorldMatrix();
	XMMATRIX XYZRotation(float x, float y, float z);

	XMFLOAT3 GetPos();
	XMFLOAT3 GetRot();
	XMFLOAT3 GetScale();

	XMFLOAT4 GetTint();
	
	void SetX(float x);
	void SetY(float y);
	void SetZ(float z);

	void SetPos(float x, float y, float z);
	void SetRot(float x, float y, float z);
	void SetScale(float x, float y, float z);

	void SetPos(XMFLOAT3 pos);
	void SetRot(XMFLOAT3 rot);
	void SetScale(XMFLOAT3 scale);
	void SetTint(XMFLOAT4 colour, int index = 0);
	void AddTintCycle(XMFLOAT4 colour);
	void SetCycleTime(float time);

	void SetColourOverlay(XMFLOAT4 colour);

	void LookAt_XZ(float x, float z);
	void LookAt(float x, float y, float z);

	void SetContext();
};

