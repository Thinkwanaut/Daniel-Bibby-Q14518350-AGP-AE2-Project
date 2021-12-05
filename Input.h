#pragma once
#include <dinput.h>
#include<map>

enum class KEYS
{
	A, B, C, D, E, F, G, H, I, J, K, L, M, N, O, P, Q, R, S, T, U, V, W, X, Y, Z,
	UP, DOWN, LEFT, RIGHT,
	N0, N1, N2, N3, N4, N5, N6, N7, N8, N9,
	NP0, NP1, NP2, NP3, NP4, NP5, NP6, NP7, NP8, NP9,
	ESC, LSHIFT, RSHIFT, SPACE, LCTRL, RCTRL, ENTER, NUMENTER
};

enum class MOUSE
{
	LCLICK, RCLICK, MCLICK
};

class Input
{
private:
	IDirectInput8* mp_DirectInput = nullptr;
	IDirectInputDevice8* mp_KeyboardDevice = nullptr;
	unsigned char m_keyboardKeyStates[256];
	unsigned char m_keyboardPrevKeyStates[256];

	IDirectInputDevice8* mp_MouseDevice = nullptr;
	DIMOUSESTATE m_mouseState;
	DIMOUSESTATE m_mousePrevState;

public:
	Input(HINSTANCE hInst, HWND hWnd, HRESULT* result);
	~Input();

	HRESULT InitialiseInput(HINSTANCE hInst, HWND hWnd);
	void ReadInputStates();
	bool KeyHeld(KEYS key);
	bool KeyPressed(KEYS key);
	bool KeyReleased(KEYS key);
	bool MouseButtonHeld(MOUSE button);
	bool MouseButtonPressed(MOUSE button);
	bool MouseButtonReleased(MOUSE button);
	float MouseRelX();
	float MouseRelY();
	int MouseWheel();
};

