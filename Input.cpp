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
	m_padPrevState = m_padState;

	HRESULT hr = mp_KeyboardDevice->GetDeviceState(sizeof(m_keyboardKeyStates), (LPVOID)&m_keyboardKeyStates);
	if (hr == DIERR_INPUTLOST || hr == DIERR_NOTACQUIRED) hr = mp_KeyboardDevice->Acquire();
	m_keysConnected = hr != DIERR_INPUTLOST && hr != DIERR_NOTACQUIRED;

	hr = mp_MouseDevice->GetDeviceState(sizeof(m_mouseState), (LPVOID)&m_mouseState);
	if (hr == DIERR_INPUTLOST || hr == DIERR_NOTACQUIRED) hr = mp_MouseDevice->Acquire();
	m_mouseConnected = hr != DIERR_INPUTLOST && hr != DIERR_NOTACQUIRED;

	m_padConnected = XInputGetState(0, &m_padState) == ERROR_SUCCESS;
}

bool Input::AnyPressed()
{
	for (int k = 0; k < NUM_KEYS; k++)
		if ((m_keyboardKeyStates[(unsigned char)k] & 0x80) && !(m_keyboardPrevKeyStates[(unsigned char)k] & 0x80)) return true;
	for (int m = 0; m < (int)MOUSE::BUTTONS; m++)
		if (MouseButtonPressed((MOUSE)m)) return true;
	for (int p = 1; p < (int)PAD::Y; p *= 2)
		if (PadButtonPressed((PAD)p)) return true;
	return false;
}

bool Input::KeyHeld(KEYS key)
{
	if (!m_keysConnected) return false;
	return (m_keyboardKeyStates[(unsigned char)key] & 0x80); // && (m_keyboardPrevKeyStates[KeyMap[key]] & 0x80)
}

bool Input::KeyPressed(KEYS key)
{
	if (!m_keysConnected) return false;
	return (m_keyboardKeyStates[(unsigned char)key] & 0x80) && !(m_keyboardPrevKeyStates[(unsigned char)key] & 0x80);
}

bool Input::KeyReleased(KEYS key)
{
	if (!m_keysConnected) return false;
	return !(m_keyboardKeyStates[(unsigned char)key] & 0x80) && (m_keyboardPrevKeyStates[(unsigned char)key] & 0x80);
}

bool Input::MouseButtonHeld(MOUSE button)
{
	if (!m_mouseConnected) return false;
	return m_mouseState.rgbButtons[(unsigned char)button]; // && m_mousePrevState.rgbButtons[(unsigned char)button]
}

bool Input::MouseButtonPressed(MOUSE button)
{
	if (!m_mouseConnected) return false;
	return m_mouseState.rgbButtons[(unsigned char)button] && !m_mousePrevState.rgbButtons[(unsigned char)button];
}

bool Input::MouseButtonReleased(MOUSE button)
{
	if (!m_mouseConnected) return false;
	return !m_mouseState.rgbButtons[(unsigned char)button] && m_mousePrevState.rgbButtons[(unsigned char)button];
}

float Input::MouseRelX()
{
	if (!m_mouseConnected) return 0.0f;
	return m_mouseState.lX;
}

float Input::MouseRelY()
{
	if (!m_mouseConnected) return 0.0f;
	return m_mouseState.lY;
}

int Input::MouseWheel()
{
	if (!m_mouseConnected) return 0;
	return m_mouseState.lZ;
}

bool Input::PadButtonHeld(PAD button)
{
	if (!m_padConnected) return false;
	return m_padState.Gamepad.wButtons & (WORD)button;
}

bool Input::PadButtonPressed(PAD button)
{
	if (!m_padConnected) return false;
	return (m_padState.Gamepad.wButtons & (WORD)button) && !(m_padPrevState.Gamepad.wButtons & (WORD)button);
}

bool Input::PadButtonReleased(PAD button)
{
	if (!m_padConnected) return false;
	return !(m_padState.Gamepad.wButtons & (WORD)button) && (m_padPrevState.Gamepad.wButtons & (WORD)button);
}

float Input::PadAxis(AXIS axis, float deadZoneMult)
{
	if (!m_padConnected) return 0.0f;
	
	XINPUT_GAMEPAD gp = m_padState.Gamepad;
	switch (axis)
	{
	case AXIS::LTRIGGER:
		return (abs(gp.bLeftTrigger) > XINPUT_GAMEPAD_TRIGGER_THRESHOLD) ? (float)gp.bLeftTrigger / (float)TRIGGER_MAX : 0.0f;
	case AXIS::RTRIGGER:
		return (abs(gp.bRightTrigger) > XINPUT_GAMEPAD_TRIGGER_THRESHOLD) ? (float)gp.bRightTrigger / (float)TRIGGER_MAX : 0.0f;
	case AXIS::LX:
		return (abs(gp.sThumbLX) > (float)XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE * deadZoneMult) ? (float)gp.sThumbLX / (float)AXIS_MAX : 0.0f;
	case AXIS::LY:
		return (abs(gp.sThumbLY) > (float)XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE * deadZoneMult) ? (float)gp.sThumbLY / (float)AXIS_MAX : 0.0f;
	case AXIS::RX:
		return (abs(gp.sThumbRX) > (float)XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE * deadZoneMult) ? (float)gp.sThumbRX / (float)AXIS_MAX : 0.0f;
	case AXIS::RY:
		return (abs(gp.sThumbRY) > (float)XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE * deadZoneMult) ? (float)gp.sThumbRY / (float)AXIS_MAX : 0.0f;
	default:
		return 0.0f;
	}
}
