#pragma once
#include <dinput.h>
#include <Xinput.h>
#include<map>

enum class KEYS
{
	A = DIK_A, B = DIK_B, C = DIK_C, D = DIK_D, E = DIK_E, F = DIK_F, G = DIK_G, H = DIK_H, I = DIK_I, 
	J = DIK_J, K = DIK_K, L = DIK_L, M = DIK_M, N = DIK_N, O = DIK_O, P = DIK_P, Q = DIK_Q, R = DIK_R, 
	S = DIK_S, T = DIK_T, U = DIK_U, V = DIK_V, W = DIK_W, X = DIK_X, Y = DIK_Y, Z = DIK_Z,
	UP = DIK_UP, DOWN = DIK_DOWN, LEFT = DIK_LEFT, RIGHT = DIK_RIGHT,
	N0 = DIK_0, N1 = DIK_1, N2 = DIK_2, N3 = DIK_3, N4 = DIK_4, N5 = DIK_5, N6 = DIK_6, N7 = DIK_8, N8 = DIK_8, N9 = DIK_9,
	NP0 = DIK_NUMPAD0, NP1 = DIK_NUMPAD1, NP2 = DIK_NUMPAD2, NP3 = DIK_NUMPAD3, NP4 = DIK_NUMPAD4, NP5 = DIK_NUMPAD5,
	NP6 = DIK_NUMPAD6, NP7 = DIK_NUMPAD7, NP8 = DIK_NUMPAD8, NP9 = DIK_NUMPAD9,
	ESC = DIK_ESCAPE, LSHIFT = DIK_LSHIFT, RSHIFT = DIK_RSHIFT, SPACE = DIK_SPACE, 
	LCTRL = DIK_LCONTROL, RCTRL = DIK_RCONTROL, ENTER = DIK_RETURN, NUMENTER = DIK_NUMPADENTER
};

enum class MOUSE
{
	LCLICK, RCLICK, MCLICK, BUTTONS
};

enum class PAD
{
	UP = XINPUT_GAMEPAD_DPAD_UP, DOWN = XINPUT_GAMEPAD_DPAD_DOWN, LEFT = XINPUT_GAMEPAD_DPAD_LEFT, RIGHT = XINPUT_GAMEPAD_DPAD_RIGHT,
	START = XINPUT_GAMEPAD_START, BACK = XINPUT_GAMEPAD_BACK, LSTICK = XINPUT_GAMEPAD_LEFT_THUMB, RSTICK = XINPUT_GAMEPAD_RIGHT_THUMB,
	LBUMPER = XINPUT_GAMEPAD_LEFT_SHOULDER, RBUMPER = XINPUT_GAMEPAD_RIGHT_SHOULDER,
	A = XINPUT_GAMEPAD_A, B = XINPUT_GAMEPAD_B, X = XINPUT_GAMEPAD_X, Y = XINPUT_GAMEPAD_Y,
};

enum class AXIS
{
	LTRIGGER, RTRIGGER, LX, LY, RX, RY, AXES
};

const int NUM_KEYS = 256, AXIS_MAX = 32767, TRIGGER_MAX = 255;

class Input
{
private:
	IDirectInput8* mp_DirectInput = nullptr;
	IDirectInputDevice8* mp_KeyboardDevice = nullptr;
	unsigned char m_keyboardKeyStates[NUM_KEYS];
	unsigned char m_keyboardPrevKeyStates[NUM_KEYS];

	IDirectInputDevice8* mp_MouseDevice = nullptr;
	DIMOUSESTATE m_mouseState, m_mousePrevState;

	XINPUT_STATE m_padState, m_padPrevState;

	bool m_keysConnected{ false }, m_mouseConnected{ false }, m_padConnected{ false };

public:
	Input(HINSTANCE hInst, HWND hWnd, HRESULT* result);
	~Input();

	HRESULT InitialiseInput(HINSTANCE hInst, HWND hWnd);
	void ReadInputStates();
	bool AnyPressed();
	bool KeyHeld(KEYS key);
	bool KeyPressed(KEYS key);
	bool KeyReleased(KEYS key);
	bool MouseButtonHeld(MOUSE button);
	bool MouseButtonPressed(MOUSE button);
	bool MouseButtonReleased(MOUSE button);
	bool PadButtonHeld(PAD button);
	bool PadButtonPressed(PAD button);
	bool PadButtonReleased(PAD button);
	float MouseRelX();
	float MouseRelY();
	int MouseWheel();
	float PadAxis(AXIS axis, float deadZoneMult = 1.0f );
};

