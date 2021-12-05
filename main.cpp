#include "Game.h"

//Rename per project
char g_GameName[100] = "AGP AE2\0";

Window* g_pWindow = nullptr;

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

int WINAPI WinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPSTR lpCmdLine, _In_ int nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

	g_pWindow = new Window(hInstance, nCmdShow, WndProc, g_GameName, 1280, 960);

	Game* newGame = new Game(hInstance, nCmdShow, g_pWindow);

	newGame->Run();

	delete newGame;
	newGame = nullptr;

	g_pWindow = nullptr; // Window deleted in game destructor

	return 0;
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	PAINTSTRUCT ps;
	HDC hdc;

	switch (message)
	{
	case WM_PAINT:
		hdc = BeginPaint(hWnd, &ps);
		EndPaint(hWnd, &ps);
		break;

	case WM_DESTROY:
		PostQuitMessage(0);
		break;

	case WM_SIZE:
		if (g_pWindow) g_pWindow->Resize(LOWORD(lParam), HIWORD(lParam));
		break;

	case WM_KEYDOWN:
		if (wParam == VK_ESCAPE) DestroyWindow(hWnd);
		break;

	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}

	return 0;
}

