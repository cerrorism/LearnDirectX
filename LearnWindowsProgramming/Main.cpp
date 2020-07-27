// LearnWindowsProgramming.cpp : Defines the entry point for the application.
//

#include "framework.h"
#include "LearnWindowsProgramming.h"
#include "GameTimer.h"
#include "Game.h"

using namespace winrt;
using namespace DirectX;

// Global Variables:
std::unique_ptr<Game> game = std::make_unique<Game>();

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{

	switch (message)
	{
	case WM_SIZE:
		game->updateWindowSize(LOWORD(lParam), HIWORD(lParam));
		break;
	case WM_LBUTTONDOWN:

	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}

void InitInstance(HINSTANCE hInstance, int nCmdShow)
{
	auto windowClassName = TEXT("LearnWindowsProgramming");
	WNDCLASSEX wc = {};

	wc.cbSize = sizeof(WNDCLASSEX);
	wc.style = CS_HREDRAW | CS_VREDRAW;
	wc.lpfnWndProc = WndProc;
	wc.hInstance = hInstance;
	wc.hIcon = LoadIcon(nullptr, IDI_APPLICATION);
	wc.hCursor = LoadCursor(nullptr, IDC_ARROW);
	wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wc.lpszClassName = windowClassName;

	check_bool(RegisterClassEx(&wc));

	screen_size defaultSize = game->defaultScreenSize();

	RECT rc = { 0, 0, static_cast<LONG>(defaultSize.width), static_cast<LONG>(defaultSize.height) };

	AdjustWindowRect(&rc, WS_OVERLAPPEDWINDOW, FALSE);

	auto hWnd = CreateWindow(windowClassName, TEXT("test window"), WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, CW_USEDEFAULT, rc.right - rc.left, rc.bottom - rc.top, nullptr, nullptr, hInstance, nullptr);
	check_bool(hWnd);
	game->setWindow(hWnd, rc.right - rc.left, rc.bottom - rc.top);

	ShowWindow(hWnd, nCmdShow);
	UpdateWindow(hWnd);
}

int APIENTRY wWinMain(HINSTANCE hInstance, HINSTANCE, LPWSTR, int nCmdShow)
{
	// Perform application initialization:
	InitInstance(hInstance, nCmdShow);

	MSG msg = {};

	while (msg.message != WM_QUIT) {
		if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE)) {
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		else {
			game->tick();
		}
	}
	game.reset();
	CoUninitialize();
	return (int)msg.wParam;
}





