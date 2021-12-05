#include "AssetManager.h"

AssetManager::AssetManager(ID3D11Device* device, ID3D11DeviceContext* context)
	: mp_d3ddevice{ device }, mp_immediateContext{ context }
{
}

AssetManager::~AssetManager()
{
	for (std::map<char*, ID3D11ShaderResourceView*>::iterator texItr = mp_textures.begin(); texItr != mp_textures.end(); texItr++)
		delete texItr->second;
	mp_textures.clear();

	for (std::map<char*, ID3D11VertexShader*>::iterator verItr = mp_VShaders.begin(); verItr != mp_VShaders.end(); verItr++)
		delete verItr->second;
	mp_VShaders.clear();

	for (std::map<char*, ID3D11PixelShader*>::iterator pixItr = mp_PShaders.begin(); pixItr != mp_PShaders.end(); pixItr++)
		delete pixItr->second;
	mp_PShaders.clear();

	for (std::map<char*, ID3D11InputLayout*>::iterator inpItr = mp_InputLayouts.begin(); inpItr != mp_InputLayouts.end(); inpItr++)
		delete inpItr->second;
	mp_InputLayouts.clear();

	for (std::map<char*, ID3D11Buffer*>::iterator bufItr = mp_Buffers.begin(); bufItr != mp_Buffers.end(); bufItr++)
		delete bufItr->second;
	mp_Buffers.clear();

	for (std::map<char*, ID3D11SamplerState*>::iterator samItr = mp_Samplers.begin(); samItr != mp_Samplers.end(); samItr++)
		delete samItr->second;
	mp_Samplers.clear();

	for (std::map<char*, ObjFileModel*>::iterator modItr = mp_models.begin(); modItr != mp_models.end(); modItr++)
		delete modItr->second;
	mp_models.clear();
}

void AssetManager::LoadTexture(char* filename)
{
	ID3D11ShaderResourceView* newTex = NULL;
	D3DX11CreateShaderResourceViewFromFile(mp_d3ddevice, filename, NULL, NULL, &newTex, NULL);
	mp_textures[filename] = newTex;
}

void AssetManager::LoadModel(char* filename)
{
	ObjFileModel* newMod = new ObjFileModel(filename, mp_d3ddevice, mp_immediateContext);
	mp_models[filename] = newMod;
}

ID3D11ShaderResourceView* AssetManager::GetTexture(char* filename)
{
    if (!CheckTexture(filename)) LoadTexture(filename);
	return mp_textures[filename];
}

ID3D11VertexShader* AssetManager::GetVShader(char* filename)
{
	if (!CheckShaders(filename)) LoadShaders(filename);
	return mp_VShaders[filename];
}

ID3D11PixelShader* AssetManager::GetPShader(char* filename)
{
	if (!CheckShaders(filename)) LoadShaders(filename);
	return mp_PShaders[filename];
}

ID3D11InputLayout* AssetManager::GetInputLayout(char* filename)
{
	if (!CheckShaders(filename)) LoadShaders(filename);
	return mp_InputLayouts[filename];
}

ID3D11Buffer* AssetManager::GetBuffer(char* filename)
{
	if (!CheckShaders(filename)) LoadShaders(filename);
	return mp_Buffers[filename];
}

ID3D11SamplerState* AssetManager::GetSampler(char* filename)
{
	if (!CheckShaders(filename)) LoadShaders(filename);
	return mp_Samplers[filename];
}

ObjFileModel* AssetManager::GetModel(char* filename)
{
	if (!CheckModel(filename)) LoadModel(filename);
	return mp_models[filename];
}

HRESULT AssetManager::LoadShaders(char* filename, bool skybox)
{
	ID3D11VertexShader* VShader;
	ID3D11PixelShader* PShader;
	ID3D11InputLayout* InputLayout;
	ID3D11Buffer* Buffer;
	ID3D11SamplerState* Sampler;

	ID3DBlob* VS, * PS, * error;

	HRESULT hr = D3DX11CompileFromFile(filename, 0, 0, "VShader", "vs_4_0", 0, 0, 0, &VS, &error, 0);
	if (error != 0)
	{
		OutputDebugStringA((char*)error->GetBufferPointer());
		error->Release();
		if (FAILED(hr)) return hr;
	}

	hr = D3DX11CompileFromFile(filename, 0, 0, "PShader", "ps_4_0", 0, 0, 0, &PS, &error, 0);
	if (error != 0)
	{
		OutputDebugStringA((char*)error->GetBufferPointer());
		error->Release();
		if (FAILED(hr)) return hr;
	}

	//Create shader objects
	hr = mp_d3ddevice->CreatePixelShader(PS->GetBufferPointer(), PS->GetBufferSize(), NULL, &PShader);
	if (FAILED(hr)) return hr;

	hr = mp_d3ddevice->CreateVertexShader(VS->GetBufferPointer(), VS->GetBufferSize(), NULL, &VShader);
	if (FAILED(hr)) return hr;

	//Create/set input layout object
	if (skybox)
	{
		D3D11_INPUT_ELEMENT_DESC iedesc[] =
		{
			{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
			{"COLOUR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
			{"TEXCOORD", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
		};
		hr = mp_d3ddevice->CreateInputLayout(iedesc, ARRAYSIZE(iedesc), VS->GetBufferPointer(), VS->GetBufferSize(), &InputLayout);
		if (FAILED(hr)) return hr;
	}
	else
	{
		D3D11_INPUT_ELEMENT_DESC iedesc[] =
		{
			{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
			{"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
			{"NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0}
		};
		hr = mp_d3ddevice->CreateInputLayout(iedesc, ARRAYSIZE(iedesc), VS->GetBufferPointer(), VS->GetBufferSize(), &InputLayout);
		if (FAILED(hr)) return hr;
	}


	D3D11_BUFFER_DESC bufferDesc;
	ZeroMemory(&bufferDesc, sizeof(bufferDesc));
	bufferDesc.Usage = D3D11_USAGE_DEFAULT;
	bufferDesc.ByteWidth = sizeof(DRAW_BUFFER);
	bufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;

	hr = mp_d3ddevice->CreateBuffer(&bufferDesc, NULL, &Buffer);
	if (FAILED(hr)) return hr;

	D3D11_SAMPLER_DESC sampler_desc;
	ZeroMemory(&sampler_desc, sizeof(sampler_desc));
	sampler_desc.Filter = (skybox) ? D3D11_FILTER_MIN_MAG_MIP_LINEAR : D3D11_FILTER_MIN_MAG_MIP_POINT;
	sampler_desc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	sampler_desc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	sampler_desc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	sampler_desc.MaxLOD = D3D11_FLOAT32_MAX;

	mp_d3ddevice->CreateSamplerState(&sampler_desc, &Sampler);

	mp_VShaders[filename] = VShader;
	mp_PShaders[filename] = PShader;
	mp_InputLayouts[filename] = InputLayout;
	mp_Buffers[filename] = Buffer;
	mp_Samplers[filename] = Sampler;

	return hr;
}

bool AssetManager::CheckTexture(char* name)
{
	return mp_textures.find(name) != mp_textures.end();
}

bool AssetManager::CheckModel(char* name)
{
	return mp_models.find(name) != mp_models.end();
}

bool AssetManager::CheckShaders(char* name)
{
	return mp_VShaders.find(name) != mp_VShaders.end();
}
