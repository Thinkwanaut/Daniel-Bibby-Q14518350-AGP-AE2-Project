#pragma once
//Order is important
#include <d3d11.h>
#include <dxgi.h>
#include <d3dx11.h>
#include <windows.h>
#include <dxerr.h>

#define _XM_NO_INSTRINSICS_
#define XM_NO_ALIGNMENT
#include <DirectXMath.h>

using namespace DirectX;
class Window
{
private:
	HINSTANCE m_hInst = NULL;
	HWND m_hWnd = NULL;

	D3D_DRIVER_TYPE			m_driverType = D3D_DRIVER_TYPE_NULL;
	D3D_FEATURE_LEVEL		m_featureLevel = D3D_FEATURE_LEVEL_11_0;
	ID3D11Device* mp_D3DDevice = NULL;
	ID3D11DeviceContext* mp_ImmediateContext = NULL;
	IDXGISwapChain* mp_SwapChain = NULL;
	ID3D11RenderTargetView* mp_BackBufferRTView = NULL;
	ID3D11RasterizerState* mp_RasterizerNone = NULL;
	ID3D11RasterizerState* mp_RasterizerBack = NULL;
	ID3D11DepthStencilView* mp_ZBuffer = NULL;

	ID3D11BlendState* mp_AlphaBlendEnable;
	ID3D11BlendState* mp_AlphaBlendDisable;

	int m_Width = 640, m_Height = 480;
	const int D3D_REFRESH_RATE = 60;
	RECT CLIENT_RECT;

	float m_ClearColour[4] = { 0.0f, 0.5f, 0.7f, 1.0f };
	bool m_Resized{ false };

public:
	Window(HINSTANCE hInstance, int nCmdShow, WNDPROC WndProc, const char* name, int width = 640, int height = 480);
	~Window();

	HRESULT InitialiseWindow(HINSTANCE hInstance, int nCmdShow, WNDPROC WndProc, const char* name, int width = 640, int height = 480);
	HRESULT InitialiseD3D();
	HRESULT InitialiseViewport(int width, int height);
	HRESULT InitialiseGraphics();
	HRESULT Resize(int width, int height);

	HINSTANCE HInst();
	HWND HWnd();

	ID3D11Device* Device();
	ID3D11DeviceContext* Context();

	int Width();
	int Height();

	bool Resized();

	void ResetContext();
	void Present();
};

