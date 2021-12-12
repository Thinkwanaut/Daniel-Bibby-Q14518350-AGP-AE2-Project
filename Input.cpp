#include "Input.h"

std::map<KEYS, unsigned char> KeyMap
{
	{KEYS::A, DIK_A}, {KEYS::B, DIK_B}, {KEYS::C, DIK_C}, {KEYS::D, DIK_D}, {KEYS::E, DIK_E}, {KEYS::F, DIK_F}, {KEYS::G, DIK_G}, {KEYS::H, DIK_H}, {KEYS::I, DIK_I},
	{KEYS::J, DIK_J}, {KEYS::K, DIK_K}, {KEYS::L, DIK_L}, {KEYS::M, DIK_M}, {KEYS::N, DIK_N}, {KEYS::O, DIK_O}, {KEYS::P, DIK_P}, {KEYS::Q, DIK_Q}, {KEYS::R, DIK_R},
	{KEYS::S, DIK_S}, {KEYS::T, DIK_T}, {KEYS::U, DIK_U}, {KEYS::V, DIK_V}, {KEYS::W, DIK_W}, {KEYS::X, DIK_X}, {KEYS::Y, DIK_Y}, {KEYS::Z, DIK_Z},

	{KEYS::N0, DIK_0}, {KEYS::N1, DIK_1}, {KEYS::N2, DIK_2}, {KEYS::N3, DIK_3}, {KEYS::N4, DIK_4},
	{KEYS::N5, DIK_5}, {KEYS::N6, DIK_6}, {KEYS::N7, DIK_7}, {KEYS::N8, DIK_8}, {KEYS::N9, DIK_9},
	{KEYS::NP0, DIK_NUMPAD0}, {KEYS::NP1, DIK_NUMPAD1}, {KEYS::NP2, DIK_NUMPAD2}, {KEYS::NP3, DIK_NUMPAD3}, {KEYS::NP4, DIK_NUMPAD4},
	{KEYS::NP5, DIK_NUMPAD5}, {KEYS::NP6, DIK_NUMPAD6}, {KEYS::NP7, DIK_NUMPAD7}, {KEYS::NP8, DIK_NUMPAD8}, {KEYS::NP9, DIK_NUMPAD9},

	{KEYS::ESC, DIK_ESCAPE}, {KEYS::LSHIFT, DIK_LSHIFT}, {KEYS::RSHIFT, DIK_RSHIFT}, {KEYS::SPACE, DIK_SPACE},
	{KEYS::LCTRL, DIK_LCONTROL}, {KEYS::RCTRL, DIK_RCONTROL}, {KEYS::ENTER, DIK_RETURN}, {KEYS::NUMENTER, DIK_NUMPADENTER}

};

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
	return (m_keyboardKeyStates[KeyMap[key]] & 0x80); // && (m_keyboardPrevKeyStates[KeyMap[key]] & 0x80)
}

bool Input::KeyPressed(KEYS key)
{
	return (m_keyboardKeyStates[KeyMap[key]] & 0x80) && !(m_keyboardPrevKeyStates[KeyMap[key]] & 0x80);
}

bool Input::KeyReleased(KEYS key)
{
	return !(m_keyboardKeyStates[KeyMap[key]] & 0x80) && (m_keyboardPrevKeyStates[KeyMap[key]] & 0x80);
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
