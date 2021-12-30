#include "Input.h"

Input::Input(HINSTANCE hInst, HWND hWnd, HRESULT* result)
{
	*result = InitialiseInput(hInst, hWnd);
}

Input::~Input()
{
	if (mp_MouseDevice)
	{
		mp_MouseDevice->Unacquire();
		mp_MouseDevice->Release();
	}
	if (mp_KeyboardDevice)
	{
		mp_KeyboardDevice->Unacquire();
		mp_KeyboardDevice->Release();
	}
	if (mp_DirectInput) mp_DirectInput->Release();
}

HRESULT Input::InitialiseInput(HINSTANCE hInst, HWND hWnd)
{
	HRESULT hr = S_OK;
	ZeroMemory(m_keyboardKeyStates, sizeof(m_keyboardKeyStates));
	ZeroMemory(m_keyboardPrevKeyStates, sizeof(m_keyboardPrevKeyStates));

	hr = DirectInput8Create(hInst, DIRECTINPUT_VERSION, IID_IDirectInput8, (void**)&mp_DirectInput, NULL);
	if (FAILED(hr)) return hr;

	hr = mp_DirectInput->CreateDevice(GUID_SysKeyboard, &mp_KeyboardDevice, NULL);
	if (FAILED(hr)) return hr;

	hr = mp_KeyboardDevice->SetDataFormat(&c_dfDIKeyboard);
	if (FAILED(hr)) return hr;

	hr = mp_KeyboardDevice->SetCooperativeLevel(hWnd, DISCL_FOREGROUND | DISCL_NONEXCLUSIVE);
	if (FAILED(hr)) return hr;

	hr = mp_DirectInput->CreateDevice(GUID_SysMouse, &mp_MouseDevice, NULL);
	if (FAILED(hr)) return hr;

	hr = mp_MouseDevice->SetDataFormat(&c_dfDIMouse);
	if (FAILED(hr)) return hr;

	hr = mp_MouseDevice->SetCooperativeLevel(hWnd, DISCL_FOREGROUND | DISCL_EXCLUSIVE);
	if (FAILED(hr)) return hr;

	hr = mp_KeyboardDevice->Acquire();
	if (FAILED(hr)) return hr;

	return hr;
}

void Input::ReadInputStates()
{
	memcpy(m_keyboardPrevKeyStates, m_keyboardKeyStates, sizeof(m_keyboardKeyStates));
	m_mousePrevState = m_mouseState;

	HRESULT hr = mp_KeyboardDevice->GetDeviceState(sizeof(m_keyboardKeyStates), (LPVOID)&m_keyboardKeyStates);
	if (hr == DIERR_INPUTLOST || hr == DIERR_NOTACQUIRED) mp_KeyboardDevice->Acquire();

	hr = mp_MouseDevice->GetDeviceState(sizeof(m_mouseState), (LPVOID)&m_mouseState);
	if (hr == DIERR_INPUTLOST || hr == DIERR_NOTACQUIRED) mp_MouseDevice->Acquire();
}

bool Input::AnyPressed()
{
	for (int k = 0; k < NUM_KEYS; k++)
		if ((m_keyboardKeyStates[(unsigned char)k] & 0x80) && !(m_keyboardPrevKeyStates[(unsigned char)k] & 0x80)) return true;
	for (int m = 0; m < (int)MOUSE::BUTTONS; m++)
		if (MouseButtonPressed((MOUSE)m)) return true;
	return false;
}

bool Input::KeyHeld(KEYS key)
{
	return (m_keyboardKeyStates[(unsigned char)key] & 0x80); // && (m_keyboardPrevKeyStates[KeyMap[key]] & 0x80)
}

bool Input::KeyPressed(KEYS key)
{
	return (m_keyboardKeyStates[(unsigned char)key] & 0x80) && !(m_keyboardPrevKeyStates[(unsigned char)key] & 0x80);
}

bool Input::KeyReleased(KEYS key)
{
	return !(m_keyboardKeyStates[(unsigned char)key] & 0x80) && (m_keyboardPrevKeyStates[(unsigned char)key] & 0x80);
}

bool Input::MouseButtonHeld(MOUSE button)
{
	return m_mouseState.rgbButtons[(unsigned char)button]; // && m_mousePrevState.rgbButtons[(unsigned char)button]
}

bool Input::MouseButtonPressed(MOUSE button)
{
	return m_mouseState.rgbButtons[(unsigned char)button] && !m_mousePrevState.rgbButtons[(unsigned char)button];
}

bool Input::MouseButtonReleased(MOUSE button)
{
	return !m_mouseState.rgbButtons[(unsigned char)button] && m_mousePrevState.rgbButtons[(unsigned char)button];
}

float Input::MouseRelX()
{
	return m_mouseState.lX;
}

float Input::MouseRelY()
{
	return m_mouseState.lY;
}

int Input::MouseWheel()
{
	return m_mouseState.lZ;
}
