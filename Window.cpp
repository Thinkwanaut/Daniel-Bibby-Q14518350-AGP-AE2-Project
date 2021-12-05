#include "Window.h"

Window::Window(HINSTANCE hInstance, int nCmdShow, WNDPROC WndProc, const char* name, int width, int height)
	: m_Width{ width }, m_Height{ height }
{
	if (FAILED(InitialiseWindow(hInstance, nCmdShow, WndProc, name, m_Width, m_Height)))
	{
		DXTRACE_MSG("Failed to create Window");
	}

	if (FAILED(InitialiseD3D()))
	{
		DXTRACE_MSG("Failed to create Device");
	}

	if (FAILED(InitialiseGraphics()))
	{
		DXTRACE_MSG("Failed to initialise graphics");
	}
}

Window::~Window()
{
	if (mp_RasterizerBack)		mp_RasterizerBack->Release();
	if (mp_RasterizerNone)		mp_RasterizerNone->Release();
	if (mp_AlphaBlendEnable)	mp_AlphaBlendEnable->Release();
	if (mp_AlphaBlendDisable)	mp_AlphaBlendDisable->Release();
	if (mp_ZBuffer)				mp_ZBuffer->Release();
	if (mp_SwapChain)			mp_SwapChain->Release();
	if (mp_ImmediateContext)	mp_ImmediateContext->Release();
	if (mp_D3DDevice)			mp_D3DDevice->Release();
	if (mp_BackBufferRTView)	mp_BackBufferRTView->Release();
}

HRESULT Window::InitialiseWindow(HINSTANCE hInstance, int nCmdShow, WNDPROC WndProc, const char* name, int width, int height)
{
	char Name[100] = "App 1.0\0";

	//Register window class
	WNDCLASSEX wcex = { 0 };
	wcex.cbSize = sizeof(WNDCLASSEX);
	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = WndProc;
	wcex.hInstance = hInstance;
	wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
	wcex.lpszClassName = Name;

	if (!RegisterClassEx(&wcex)) return E_FAIL;

	//Create Window
	m_hInst = hInstance;
	RECT rc = { 0, 0, width, height };
	AdjustWindowRect(&rc, WS_OVERLAPPEDWINDOW, FALSE);
	m_hWnd = CreateWindow(Name, name, WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, CW_USEDEFAULT, rc.right - rc.left, rc.bottom - rc.top,
		NULL, NULL, hInstance, NULL);
	if (!m_hWnd) return E_FAIL;

	ShowWindow(m_hWnd, nCmdShow);

	return S_OK;
}

HRESULT Window::Resize(int width, int height)
{
	if (!mp_SwapChain) return E_FAIL;

	mp_ImmediateContext->OMSetRenderTargets(0, 0, 0);

	//Release all buffers
	if (mp_BackBufferRTView) mp_BackBufferRTView->Release();
	if (mp_ZBuffer) mp_ZBuffer->Release();

	//Preserve existing buffer count/format
	//Automatically choose the width/height to match client rect for HWNDs
	HRESULT hr;
	hr = mp_SwapChain->ResizeBuffers(0, 0, 0, DXGI_FORMAT_UNKNOWN, 0);

	if (FAILED(hr)) return hr;

	hr = InitialiseViewport(width, height);
	if (FAILED(hr)) return hr;

	m_Resized = true;

	return S_OK;
}

HINSTANCE Window::HInst()
{
	return m_hInst;
}

HWND Window::HWnd()
{
	return m_hWnd;
}

ID3D11Device* Window::Device()
{
	return mp_D3DDevice;
}

ID3D11DeviceContext* Window::Context()
{
	return mp_ImmediateContext;
}

int Window::Width()
{
	return m_Width;
}

int Window::Height()
{
	return m_Height;
}

bool Window::Resized()
{
	return m_Resized;
}

void Window::Rainbow(float adjust)
{
	if (m_ClearColour[0] < 1.0f && m_ClearColour[1] == 0 && m_ClearColour[2] == 0) m_ClearColour[0] += m_ColourCycle * adjust;
	else if (m_ClearColour[0] == 1.0f && m_ClearColour[1] < 1.0f && m_ClearColour[2] == 0) m_ClearColour[1] += m_ColourCycle * adjust;
	else if (m_ClearColour[0] > 0 && m_ClearColour[1] == 1.0f && m_ClearColour[2] == 0) m_ClearColour[0] -= m_ColourCycle * adjust;
	else if (m_ClearColour[0] == 0 && m_ClearColour[1] == 1.0f && m_ClearColour[2] < 1.0f) m_ClearColour[2] += m_ColourCycle * adjust;
	else if (m_ClearColour[0] == 0 && m_ClearColour[1] > 0 && m_ClearColour[2] == 1.0f) m_ClearColour[1] -= m_ColourCycle * adjust;
	else if (m_ClearColour[0] < 1.0f && m_ClearColour[1] == 0 && m_ClearColour[2] == 1.0f) m_ClearColour[0] += m_ColourCycle * adjust;
	else if (m_ClearColour[0] == 1.0f && m_ClearColour[1] == 0 && m_ClearColour[2] > 0) m_ClearColour[2] -= m_ColourCycle * adjust;
	else
		for (int c = 0; c < COLOUR_PARTS; c++) m_ClearColour[c] -= m_ColourCycle * adjust;

	for (int c = 0; c < COLOUR_PARTS; c++) m_ClearColour[c] = max(min(m_ClearColour[c], 1.0f), 0.0f);
}

void Window::Clear()
{
	mp_ImmediateContext->ClearRenderTargetView(mp_BackBufferRTView, m_ClearColour);
}

void Window::Clear(float r, float g, float b)
{
	float colour[4] = { r, g, b, 1 };
	mp_ImmediateContext->ClearRenderTargetView(mp_BackBufferRTView, colour);
}

void Window::ResetContext()
{
	mp_ImmediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	mp_ImmediateContext->RSSetState(mp_RasterizerNone);
	mp_ImmediateContext->ClearRenderTargetView(mp_BackBufferRTView, m_ClearColour);
	mp_ImmediateContext->ClearDepthStencilView(mp_ZBuffer, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
	mp_ImmediateContext->OMSetBlendState(mp_AlphaBlendEnable, 0, 0xffffffff);
}

void Window::Present()
{
	mp_SwapChain->Present(0, 0);
	m_Resized = false;
}

//Create D3D device && swap chain

HRESULT Window::InitialiseD3D()
{
	HRESULT hr = S_OK;

	GetClientRect(m_hWnd, &CLIENT_RECT);
	UINT width = CLIENT_RECT.right - CLIENT_RECT.left;
	UINT height = CLIENT_RECT.bottom - CLIENT_RECT.top;

	UINT createDeviceFlags = 0;

#ifndef NDEBUG
	createDeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

	D3D_DRIVER_TYPE driverTypes[] = { D3D_DRIVER_TYPE_HARDWARE, D3D_DRIVER_TYPE_WARP, D3D_DRIVER_TYPE_REFERENCE };
	UINT numDriverTypes = ARRAYSIZE(driverTypes);

	D3D_FEATURE_LEVEL featureLevels[] = { D3D_FEATURE_LEVEL_11_0, D3D_FEATURE_LEVEL_10_1, D3D_FEATURE_LEVEL_10_0 };
	UINT numFeatureLevels = ARRAYSIZE(featureLevels);

	DXGI_SWAP_CHAIN_DESC sd;
	ZeroMemory(&sd, sizeof(sd));
	sd.BufferCount = 1;
	sd.BufferDesc.Width = width;
	sd.BufferDesc.Height = height;
	sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	sd.BufferDesc.RefreshRate.Numerator = D3D_REFRESH_RATE;
	sd.BufferDesc.RefreshRate.Denominator = 1;
	sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	sd.OutputWindow = m_hWnd;
	sd.SampleDesc.Count = 1;
	sd.SampleDesc.Quality = 0;
	sd.Windowed = true;

	for (UINT driverTpyeIndex = 0; driverTpyeIndex < numDriverTypes; driverTpyeIndex++)
	{
		m_driverType = driverTypes[driverTpyeIndex];
		hr = D3D11CreateDeviceAndSwapChain(NULL, m_driverType, NULL,
			createDeviceFlags, featureLevels, numFeatureLevels, D3D11_SDK_VERSION,
			&sd, &mp_SwapChain, &mp_D3DDevice, &m_featureLevel, &mp_ImmediateContext);
		if (SUCCEEDED(hr)) break;
	}

	if (FAILED(hr)) return hr;

	hr = InitialiseViewport(width, height);
	if (FAILED(hr)) return hr;

	D3D11_BLEND_DESC b;
	b.RenderTarget[0].BlendEnable = TRUE;
	b.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
	b.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
	b.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
	b.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
	b.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
	b.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
	b.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
	b.IndependentBlendEnable = FALSE;
	b.AlphaToCoverageEnable = FALSE;

	mp_D3DDevice->CreateBlendState(&b, &mp_AlphaBlendEnable);

	return S_OK;
}

HRESULT Window::InitialiseViewport(int width, int height)
{
	HRESULT hr = S_OK;

	//Get pointer to back buffer texture
	ID3D11Texture2D* pBackBufferTexture;
	hr = mp_SwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&pBackBufferTexture);

	if (FAILED(hr)) return hr;

	//Use the back buffer texture pointer to create the render target view
	hr = mp_D3DDevice->CreateRenderTargetView(pBackBufferTexture, NULL, &mp_BackBufferRTView);
	pBackBufferTexture->Release();

	if (FAILED(hr)) return hr;

	//Create Z buffer texture
	D3D11_TEXTURE2D_DESC tex2dDesc;
	ZeroMemory(&tex2dDesc, sizeof(tex2dDesc));

	tex2dDesc.Width = width;
	tex2dDesc.Height = height;
	tex2dDesc.ArraySize = 1;
	tex2dDesc.MipLevels = 1;
	tex2dDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	tex2dDesc.SampleDesc.Count = 1;
	tex2dDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	tex2dDesc.Usage = D3D11_USAGE_DEFAULT;

	ID3D11Texture2D* pZBufferTexture;
	hr = mp_D3DDevice->CreateTexture2D(&tex2dDesc, NULL, &pZBufferTexture);
	if (FAILED(hr)) return hr;

	//Create the Z buffer
	D3D11_DEPTH_STENCIL_VIEW_DESC dsvDesc;
	ZeroMemory(&dsvDesc, sizeof(dsvDesc));

	dsvDesc.Format = tex2dDesc.Format;
	dsvDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;

	mp_D3DDevice->CreateDepthStencilView(pZBufferTexture, &dsvDesc, &mp_ZBuffer);
	pZBufferTexture->Release();

	//Set the render target view
	mp_ImmediateContext->OMSetRenderTargets(1, &mp_BackBufferRTView, mp_ZBuffer);

	m_Width = width;
	m_Height = height;

	//Set the viewport
	D3D11_VIEWPORT viewport;

	viewport.TopLeftX = 0;
	viewport.TopLeftY = 0;
	viewport.Width = (float)width;
	viewport.Height = (float)height;
	viewport.MinDepth = 0.0f;
	viewport.MaxDepth = 1.0f;

	mp_ImmediateContext->RSSetViewports(1, &viewport);

	return hr;
}

HRESULT Window::InitialiseGraphics()
{
	HRESULT hr = S_OK;

	D3D11_RASTERIZER_DESC rastdesc;
	ZeroMemory(&rastdesc, sizeof(D3D11_RASTERIZER_DESC));
	rastdesc.FillMode = D3D11_FILL_SOLID;
	rastdesc.CullMode = D3D11_CULL_NONE;
	rastdesc.FrontCounterClockwise = false;
	rastdesc.DepthBias = false;
	rastdesc.DepthBiasClamp = 0;
	rastdesc.SlopeScaledDepthBias = 0;
	rastdesc.DepthClipEnable = true;
	rastdesc.MultisampleEnable = true;
	rastdesc.AntialiasedLineEnable = true;
	hr = mp_D3DDevice->CreateRasterizerState(&rastdesc, &mp_RasterizerNone);
	if (FAILED(hr)) return hr;

	ZeroMemory(&rastdesc, sizeof(D3D11_RASTERIZER_DESC));
	rastdesc.FillMode = D3D11_FILL_SOLID;
	rastdesc.CullMode = D3D11_CULL_BACK;
	rastdesc.FrontCounterClockwise = false;
	rastdesc.DepthBias = false;
	rastdesc.DepthBiasClamp = 0;
	rastdesc.SlopeScaledDepthBias = 0;
	rastdesc.DepthClipEnable = true;
	rastdesc.MultisampleEnable = false;
	rastdesc.AntialiasedLineEnable = false;
	hr = mp_D3DDevice->CreateRasterizerState(&rastdesc, &mp_RasterizerBack);
	if (FAILED(hr)) return hr;

	return S_OK;
}
