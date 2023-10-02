
#include "framework.h"
#include "Lab1.h"
#include <windows.h>

#pragma comment(lib, "Msimg32.lib")

LRESULT CALLBACK WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
void DrawPicture(HANDLE& hBmp, BITMAP& bitmap, POINT& bitmapCoordinates);

const int BITMAP_SIZE = 200;
HDC hdcBack;
HANDLE hbmBack;
RECT clientFrame;

POINT imageCoorg;
POINT mouseCoord;
int offset;
int autoOffsetX = 10;
int autoOffsetY = 10;
RECT clientRect;
bool shouldAvtoMove = false;
UINT_PTR timer;

void InitializeBack(HWND hWnd, int width, int height)
{
	HDC hdcWindow;
	hdcWindow = GetDC(hWnd);
	hdcBack = CreateCompatibleDC(hdcWindow);
	hbmBack = CreateCompatibleBitmap(hdcWindow, width, height);
	ReleaseDC(hWnd, hdcWindow);

	SaveDC(hdcBack);
	SelectObject(hdcBack, hbmBack);
}

void FreeBack()
{
	if (hdcBack)
	{
		RestoreDC(hdcBack, -1);
		DeleteObject(hbmBack);
		DeleteDC(hdcBack);
		hdcBack = 0;
	}
}

int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR pCmdLine, int nCmdShow)
{
	WNDCLASSEX wcex;
	HWND hWnd;
	MSG msg;

	wcex.cbSize = sizeof(WNDCLASSEX);
	wcex.style = CS_VREDRAW | CS_HREDRAW;
	wcex.lpfnWndProc = WndProc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = hInstance;
	wcex.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_LAB1));
	wcex.hCursor = LoadCursorW(NULL, IDC_ARROW);
	wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wcex.lpszMenuName = NULL;
	wcex.lpszClassName = L"FirstLab";
	wcex.hIconSm = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

	if (!RegisterClassEx(&wcex))
	{
		return 0;
	}

	hWnd = CreateWindowEx(0, L"FirstLab", L"Lab1", WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT,
		CW_USEDEFAULT, CW_USEDEFAULT, NULL, NULL, hInstance, NULL);

	ShowWindow(hWnd, nCmdShow);
	UpdateWindow(hWnd);

	while (GetMessage(&msg, NULL, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	return msg.wParam;
}

void moveImageUsingKeys(HWND hWnd, WPARAM wParam, bool& isMoveKey) {
	switch (wParam)
	{
	case VK_LEFT:
	case 0x41:
		imageCoorg.x - offset < clientRect.left ? imageCoorg.x += offset : imageCoorg.x -= offset;
		isMoveKey = true;
		break;
	case VK_RIGHT:
	case 0x44:
		imageCoorg.x + offset > clientRect.right - BITMAP_SIZE ? imageCoorg.x -= offset : imageCoorg.x += offset;
		isMoveKey = true;
		break;
	case VK_UP:
	case 0x57:
		imageCoorg.y - offset < clientRect.top ? imageCoorg.y += offset : imageCoorg.y -= offset;
		isMoveKey = true;
		break;
	case VK_DOWN:
	case 0x53:
		imageCoorg.y + offset > clientRect.bottom - BITMAP_SIZE ? imageCoorg.y -= offset : imageCoorg.y += offset;
		isMoveKey = true;
		break;
	case 0x20:

		shouldAvtoMove = true;
		timer = SetTimer(hWnd, 1, 30, NULL);
		break;
	case 0x0D:
		shouldAvtoMove = false;
		KillTimer(hWnd, 0);
	}
}

void moveImageUsingMouseWheel(WPARAM wParam) {
	int direction = GET_WHEEL_DELTA_WPARAM(wParam);
	if (LOWORD(wParam) & MK_SHIFT)
	{
		(imageCoorg.x - (direction / 25) >= clientRect.left) && (imageCoorg.x - (direction / 25) <= clientRect.right - BITMAP_SIZE) ? imageCoorg.x -= (direction / 25) : imageCoorg.x += (direction / 25);
	}
	else
	{
		(imageCoorg.y + (direction / 25) >= clientRect.top) && (imageCoorg.y + (direction / 25) <= clientRect.bottom - BITMAP_SIZE) ? imageCoorg.y += (direction / 25) : imageCoorg.y -= (direction / 25);
	}
}

bool leftMouseButtonClicked(LPARAM lParam) {
	int x = LOWORD(lParam);
	int y = HIWORD(lParam);
	mouseCoord.x = x - imageCoorg.x;
	mouseCoord.y = y - imageCoorg.y;
	if ((mouseCoord.x >= 0) && (mouseCoord.x <= BITMAP_SIZE) && (mouseCoord.y >= 0) && (mouseCoord.y <= BITMAP_SIZE))
		return true;
	return false;
}

void moveImageUsingMouse(LPARAM lParam) {
	int x = LOWORD(lParam);
	int y = HIWORD(lParam);
	if ((x - mouseCoord.x >= clientRect.left) && (x - mouseCoord.x <= clientRect.right - BITMAP_SIZE))
		imageCoorg.x = x - mouseCoord.x;
	else
		if (x - mouseCoord.x < clientRect.left)
			imageCoorg.x = 0;
		else
			imageCoorg.x = clientRect.right - BITMAP_SIZE;

	if ((y - mouseCoord.y >= clientRect.top) && (y - mouseCoord.y <= clientRect.bottom - BITMAP_SIZE))
		imageCoorg.y = y - mouseCoord.y;
	else
		if (y - mouseCoord.y < clientRect.top)
			imageCoorg.y = 0;
		else
			imageCoorg.y = clientRect.bottom - BITMAP_SIZE;
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	PAINTSTRUCT ps;
	static HDC hDC;
	static BITMAP image;
	static HANDLE hBmp;
	static bool isMovingUsingMouse;
	GetClientRect(hWnd, &clientRect);

	switch (uMsg)
	{
	case WM_CREATE:
	{
		hBmp = LoadImage(NULL, L"C:\\Users\\pauliuchenka\\Desktop\\Ника\\ОСиСП\\Lab1\\Lab1\\penguin.bmp", IMAGE_BITMAP, 287, 301, LR_LOADFROMFILE);
		GetObject(hBmp, sizeof(image), &image);

		imageCoorg = { 100, 100 };
		isMovingUsingMouse = false;
		offset = 10;
		break;
	}
	case WM_DESTROY:
	{
		FreeBack();
		PostQuitMessage(0);
		break;
	}
	case WM_PAINT:
	{
		DrawPicture(hBmp, image, imageCoorg);

		hDC = BeginPaint(hWnd, &ps);
		//SetBkColor(hDC, RGB(255, 255, 255));	// все 1 --> 0xFFFFFF
		BitBlt(hDC, 0, 0, clientFrame.right - clientFrame.left, clientFrame.bottom - clientFrame.top, hdcBack, 0, 0, SRCCOPY);
		EndPaint(hWnd, &ps);
		break;
	}
	case WM_SIZE:
	{
		GetClientRect(hWnd, &clientFrame);
		FreeBack();
		InitializeBack(hWnd, clientFrame.right - clientFrame.left, clientFrame.bottom - clientFrame.top);
		InvalidateRect(hWnd, NULL, FALSE);
		break;
	}
	case WM_KEYDOWN:
	{
		bool isMoveKey = false;

		if (shouldAvtoMove) {
			addSpeedWithKeys(hWnd, wParam);
		}
		else {
			moveImageUsingKeys(hWnd, wParam, isMoveKey);
		}

		if (isMoveKey && !timer)
			timer = SetTimer(hWnd, 0, 1 * 1000, NULL);
		InvalidateRect(hWnd, NULL, FALSE);
		break;
	}
	case WM_MOUSEHWHEEL:
	{
		if (isMovingUsingMouse)
			break;
		int direction = GET_WHEEL_DELTA_WPARAM(wParam);
		(imageCoorg.x - (direction / 25) >= clientRect.left) && (imageCoorg.x - (direction / 25) <= clientRect.right - BITMAP_SIZE) ? imageCoorg.x -= (direction / 25) : imageCoorg.x += (direction / 25);
		InvalidateRect(hWnd, NULL, FALSE);
		break;
	}
	case WM_MOUSEWHEEL:
	{
		if (isMovingUsingMouse)
			break;

		moveImageUsingMouseWheel(wParam);
		InvalidateRect(hWnd, NULL, FALSE);
		break;
	}
	case WM_LBUTTONDOWN:
	{
		isMovingUsingMouse = leftMouseButtonClicked(lParam);
		break;
	}
	case WM_MOUSEMOVE:
	{
		if (isMovingUsingMouse)
		{
			moveImageUsingMouse(lParam);
			InvalidateRect(hWnd, NULL, FALSE);
		}
		break;
	}
	case WM_LBUTTONUP:
	{
		isMovingUsingMouse = false;
		break;
	}
	case WM_GETMINMAXINFO:
	{
		MINMAXINFO* pInfo = (MINMAXINFO*)lParam;
		POINT min = { imageCoorg.x + BITMAP_SIZE + 50, imageCoorg.y + BITMAP_SIZE + 50 };
		pInfo->ptMinTrackSize = min;
		break;
	}
	}
	return DefWindowProc(hWnd, uMsg, wParam, lParam);
}

void DrawPicture(HANDLE& hBmp, BITMAP& bitmap, POINT& bitmapCoordinates)
{
	// Save dc to place image 
	SaveDC(hdcBack);
	FillRect(hdcBack, &clientFrame, (HBRUSH)(COLOR_WINDOW + 1));

	HDC hMemDC = CreateCompatibleDC(hdcBack);
	HBITMAP hOldBmp = (HBITMAP)SelectObject(hMemDC, hBmp);
	if (hOldBmp)
	{
		SetMapMode(hMemDC, GetMapMode(hdcBack));
		TransparentBlt(hdcBack, bitmapCoordinates.x, bitmapCoordinates.y, BITMAP_SIZE, BITMAP_SIZE, hMemDC, 0, 0, bitmap.bmWidth, bitmap.bmHeight, RGB(0, 0, 0));
		SelectObject(hMemDC, hOldBmp);
	}
	DeleteDC(hMemDC);

	// Replace context with image
	RestoreDC(hdcBack, -1);
}
