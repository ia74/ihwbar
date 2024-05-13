#include <windows.h>
#include <time.h>
#include <fstream>
#include <iostream>
#include <io.h>
#include <fcntl.h>

#include "bar.h"
#include "BarConfiguration.hpp"

#include "EditorDialog.cpp"
#include "Boot.hpp"

#include "UserWidgets.cpp"

#include <thread>

#define IDT_TIMER1 1

void CreateConsole()
{
	AllocConsole();
	freopen("CONOUT$", "w", stdout);
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	HINSTANCE hInstance;
	int UserHeight;
	MINMAXINFO *mmi;
	switch (msg)
	{
	case WM_CREATE:
		SetTimer(hwnd, IDT_TIMER1, 1000, NULL);
		return 0;
	case WM_TIMER:
		InvalidateRect(hwnd, NULL, TRUE);
		return 0;
	case WM_KEYDOWN:
		switch (wParam)
		{
		case VK_ESCAPE:
			PostMessage(hwnd, WM_CLOSE, 0, 0);
			break;
		case VK_F1:
			hInstance = GetModuleHandle(NULL);
			DialogBox(hInstance, MAKEINTRESOURCE(IDD_DIALOG1), hwnd, DialogProc);
			break;
		case KB_r:
		case KB_R:
			SetWindowPos(hwnd, HWND_TOPMOST, CW_USEDEFAULT, CW_USEDEFAULT, UserDisplayWidth, GetConfigInt("Bar Height", 100), SWP_NOMOVE);
			break;
		}
		return 0;
	case WM_ERASEBKGND:
	{
		HDC hdc = (HDC)wParam;
		RECT rect;
		GetClientRect(hwnd, &rect);
		HBRUSH hbr = CreateSolidBrush(RGB(GetConfigColor("Red", 255), GetConfigColor("Green", 255), GetConfigColor("Blue", 255))); // Change the color as needed
		FillRect(hdc, &rect, hbr);
		DeleteObject(hbr);
		return 1;
	}
	case WM_PAINT:
	{

		PAINTSTRUCT ps;
		HDC hdc = BeginPaint(hwnd, &ps);

		RECT rect;
		GetClientRect(hwnd, &rect);

		HDC hdcBuffer = CreateCompatibleDC(hdc);
		HBITMAP hbmBuffer = CreateCompatibleBitmap(hdc, rect.right - rect.left, rect.bottom - rect.top);
		HBITMAP hbmOldBuffer = (HBITMAP)SelectObject(hdcBuffer, hbmBuffer);

		int r, g, b;
		r = GetConfigColor("Red", 255);
		g = GetConfigColor("Green", 255);
		b = GetConfigColor("Blue", 255);

		HBRUSH hbr = CreateSolidBrush(RGB(r, g, b));
		FillRect(hdcBuffer, &rect, hbr);
		DeleteObject(hbr);

		time_t now = time(0);
		char nowF[100];
		strftime(nowF, 100, "%Y-%m-%d %H:%M:%S", localtime(&now));

		SetBkMode(hdcBuffer, TRANSPARENT);
		SetTextColor(hdcBuffer, RGB(GetConfigColor("Text Red", 0), GetConfigColor("Text Green", 0), GetConfigColor("Text Blue", 0)));

		DrawUserWidgets(hwnd, hdcBuffer, rect);

		DrawText(hdcBuffer, nowF, -1, &rect, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
		
		BitBlt(hdc, 0, 0, rect.right - rect.left, rect.bottom - rect.top, hdcBuffer, 0, 0, SRCCOPY);

		SelectObject(hdcBuffer, hbmOldBuffer);
		DeleteObject(hbmBuffer);
		DeleteDC(hdcBuffer);

		EndPaint(hwnd, &ps);
		return 0;
	}
	case WM_DESTROY:
		KillTimer(hwnd, IDT_TIMER1);
		PostQuitMessage(0);
		return 0;
	}
	return DefWindowProc(hwnd, msg, wParam, lParam);
}

LRESULT CALLBACK KeyboardProc(int nCode, WPARAM wParam, LPARAM lParam) {
    if (nCode == HC_ACTION && wParam == WM_KEYDOWN) {
        KBDLLHOOKSTRUCT* p = (KBDLLHOOKSTRUCT*)lParam;
        switch (p->vkCode) {
			case KB_h:
			case KB_H:
				if (!(GetAsyncKeyState(VK_CONTROL) & 0x8000) ||
            	(!(GetAsyncKeyState(VK_LWIN) & 0x8000) || (GetAsyncKeyState(VK_RWIN) & 0x8000))) break;
				// hide or show bar
				HWND hwnd = FindWindow("IHWBar", "Ready for keybind input");
				if (IsWindowVisible(hwnd)) {
					ShowWindow(hwnd, SW_HIDE);
				} else {
					ShowWindow(hwnd, SW_SHOW);
				}
				break;
        }
    }
    return CallNextHookEx(NULL, nCode, wParam, lParam);
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	if (GetConfigInt("ShowConsole", 1) == 1)
	{
		CreateConsole();
	}
	std::cout << "Console created! Disable it by setting ShowConsole in the config to 0." << std::endl;
	std::cout << "Loading config from config.ihw" << std::endl;
	WNDCLASS wc = {0};
	wc.lpfnWndProc = WndProc;
	wc.hInstance = hInstance;
	wc.lpszClassName = "IHWBar";
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);

	int r, g, b, UserHeight, ShowConsole, tcr, tcg, tcb;
	Boot::Init(r, g, b, UserHeight, ShowConsole, tcr, tcg, tcb);

	wc.hbrBackground = CreateSolidBrush(RGB(r, g, b)); // Add this line

	std::cout << "Starting global keyboard hook" << std::endl;
	HHOOK hHook = SetWindowsHookEx(WH_KEYBOARD_LL, KeyboardProc, GetModuleHandle(NULL), 0);

	Boot::Welcome();

	RegisterClass(&wc);
	HWND hwnd = CreateWindowEx(
		WS_EX_TOPMOST | WS_EX_TOOLWINDOW,
		"IHWBar", 
		"Ready for keybind input",
		WS_POPUP,
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		UserDisplayWidth,
		UserHeight,
		NULL,
		NULL,
		hInstance,
		NULL);
	ShowWindow(hwnd, nCmdShow);
	UpdateWindow(hwnd);

	MSG msg;
	while (GetMessage(&msg, NULL, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	UnhookWindowsHookEx(hHook);
	return 0;
}