#pragma once
#include <d3d11.h>
#include <d3dx11.h>
#include <dxerr.h>
#define _XM_NO_INSTRINSICS_
#define XM_NO_ALIGNMENT
#include <DirectXMath.h>

#include <map>

#include "objfilemodel.h"

using namespace DirectX;
struct DRAW_BUFFER
{
	XMMATRIX WorldViewProjection;
	XMVECTOR base_colour;
	XMVECTOR directional_light_vector;
	XMVECTOR directional_light_colour;
	XMVECTOR ambient_light_colour;
	XMVECTOR point_light_position;
	XMVECTOR point_light_colour;
};
class AssetManager
{
private:
	std::map<char*, ID3D11ShaderResourceView*> mp_textures;
	std::map<char*, ID3D11VertexShader*> mp_VShaders;
	std::map<char*, ID3D11PixelShader*> mp_PShaders;
	std::map<char*, ID3D11InputLayout*> mp_InputLayouts;
	std::map<char*, ID3D11Buffer*> mp_Buffers;
	std::map<char*, ID3D11SamplerState*> mp_Samplers;
	std::map<char*, ObjFileModel*> mp_models;

	ID3D11Device* mp_d3ddevice = NULL;
	ID3D11DeviceContext* mp_immediateContext = NULL;

public:
	AssetManager(ID3D11Device* device, ID3D11DeviceContext* context);
	~AssetManager();

	void LoadTexture(char* filename);
	HRESULT LoadShaders(char* filename);
	void LoadModel(char* filename);

	ID3D11ShaderResourceView* GetTexture(char* filename);
	ID3D11VertexShader* GetVShader(char* filename);
	ID3D11PixelShader* GetPShader(char* filename);
	ID3D11InputLayout* GetInputLayout(char* filename);
	ID3D11Buffer* GetBuffer(char* filename);
	ID3D11SamplerState* GetSampler(char* filename);
	ObjFileModel* GetModel(char* filename);

	bool CheckTexture(char* name);
	bool CheckModel(char* name);
	bool CheckShaders(char* name);
};

