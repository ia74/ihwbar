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

#include "Global.h"
int paint_r, paint_g, paint_b, paint_tr, paint_tg, paint_tb;
bool debug = false;
char *cfg_FILE_W = ".\\widgets.ihw";
char *CONFIG_FILE = ".\\config.ihw";

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
	int UserHeight, bw, by;
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
			std::cout << "Reloading paint colors" << std::endl;
			paint_r = GetConfigColor("Red", 255);
			paint_g = GetConfigColor("Green", 255);
			paint_b = GetConfigColor("Blue", 255);
			paint_tr = GetConfigColor("Text Red", 0);
			paint_tg = GetConfigColor("Text Green", 0);
			paint_tb = GetConfigColor("Text Blue", 0);
			std::cout << "Reloading bar position" << std::endl;
			bw = GetConfigInt("Bar Width Offset", 0);
			by = GetConfigInt("Bar Y", 0);
			UserHeight = GetConfigInt("Bar Height", 25);
			ReloadConfigs();
			std::cout << "Setting window pos" << std::endl;
			SetWindowPos(hwnd, HWND_TOPMOST,
						 bw / 2,
						 by,
						 UserDisplayWidth - bw,
						 UserHeight, 0);
			break;
		}
		return 0;
	case WM_ERASEBKGND:
	{
		HDC hdc = (HDC)wParam;
		RECT rect;
		GetClientRect(hwnd, &rect);
		HBRUSH hbr = CreateSolidBrush(RGB(paint_r, paint_g, paint_b)); // Change the color as needed
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

		HBRUSH hbr = CreateSolidBrush(RGB(paint_r, paint_g, paint_b));
		FillRect(hdcBuffer, &rect, hbr);
		DeleteObject(hbr);

		DrawUserWidgets(hwnd, hdcBuffer, rect);

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

LRESULT CALLBACK KeyboardProc(int nCode, WPARAM wParam, LPARAM lParam)
{
	if (nCode == HC_ACTION && wParam == WM_KEYDOWN)
	{
		KBDLLHOOKSTRUCT *p = (KBDLLHOOKSTRUCT *)lParam;
		HWND hwnd = FindWindow("IHWBar", "Ready for keybind input");
		switch (p->vkCode)
		{
		case KB_h:
		case KB_H:
			if (!(GetAsyncKeyState(VK_CONTROL) & 0x8000) ||
				(!(GetAsyncKeyState(VK_LWIN) & 0x8000) || (GetAsyncKeyState(VK_RWIN) & 0x8000)))
				break;
			// hide or show bar
			if (IsWindowVisible(hwnd))
			{
				ShowWindow(hwnd, SW_HIDE);
			}
			else
			{
				ShowWindow(hwnd, SW_SHOW);
			}
			break;
		case KB_r:
		case KB_R:
			if (!(GetAsyncKeyState(VK_CONTROL) & 0x8000) ||
				(!(GetAsyncKeyState(VK_LWIN) & 0x8000) || (GetAsyncKeyState(VK_RWIN) & 0x8000)))
				break;
			SendMessage(hwnd, WM_KEYDOWN, KB_R, 0);
			break;
		case VK_ESCAPE:
			if (!(GetAsyncKeyState(VK_CONTROL) & 0x8000) ||
				(!(GetAsyncKeyState(VK_LWIN) & 0x8000) || (GetAsyncKeyState(VK_RWIN) & 0x8000)))
				break;
			SendMessage(hwnd, WM_KEYDOWN, VK_ESCAPE, 0);
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
	else if (debug)
	{
		CreateConsole();
	}

	int argc;
	LPWSTR *argv = CommandLineToArgvW(GetCommandLineW(), &argc);

	for (int i = 0; i < argc; i++)
	{
		char arg[256];
		wcstombs(arg, argv[i], 256);

        if (strcmp(arg, "-d") == 0)
        {
            debug = true;
        }
        if (strcmp(arg, "-c:c") == 0 && i + 1 < argc)
        {
            char config_file[256];
            wcstombs(config_file, argv[i + 1], 256);

            std::cout << "Loading custom config file: " << config_file << std::endl;
			CONFIG_FILE = new char[strlen(config_file) + 1];
			strcpy(CONFIG_FILE, config_file);
			i++;
        }
        if (strcmp(arg, "-c:w") == 0 && i + 1 < argc)
        {
            char widget_file[256];
            wcstombs(widget_file, argv[i + 1], 256);

            std::cout << "Loading custom widget file: " << widget_file << std::endl;
			cfg_FILE_W = new char[strlen(widget_file) + 1];
			strcpy(cfg_FILE_W, widget_file);
            i++;
        }
	}

	if (debug)
	{
		time_t now = time(0);
		char nowF[100];
		strftime(nowF, 100, "%Y-%m-%d %H:%M:%S", localtime(&now));
		std::cout << "[DEBUG] Console created at " << nowF << std::endl;
	}
	std::cout << "Console created! Disable it by setting ShowConsole in the config to 0." << std::endl;
	std::cout << "Loading bar config from " << CONFIG_FILE << std::endl;
	std::cout << "Loading widget config from " << CONFIG_FILE << std::endl;
	WNDCLASS wc = {0};
	wc.lpfnWndProc = WndProc;
	wc.hInstance = hInstance;
	wc.lpszClassName = "IHWBar";
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);

	int r, g, b, UserHeight, ShowConsole, tcr, tcg, tcb, bx, by, bw;
	Boot::Init(r, g, b, UserHeight, ShowConsole, tcr, tcg, tcb, bx, by, bw);

	wc.hbrBackground = CreateSolidBrush(RGB(r, g, b)); // Add this line

	if (debug)
	{
		std::cout << "Starting global keyboard hook" << std::endl;
	}
	HHOOK hHook = SetWindowsHookEx(WH_KEYBOARD_LL, KeyboardProc, GetModuleHandle(NULL), 0);

	Boot::Welcome();

	if (debug)
	{
		std::cout << "Registering window class" << std::endl;
	}

	RegisterClass(&wc);
	HWND hwnd = CreateWindowEx(
		WS_EX_TOPMOST | WS_EX_TOOLWINDOW,
		"IHWBar",
		"Ready for keybind input",
		WS_POPUP,
		bw / 2,
		by,
		UserDisplayWidth - bw,
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
	if (GetConfigInt("ShowConsole", 1) == 1)
		std::cout << "Bye!" << std::endl;
	FreeConsole();

	UnhookWindowsHookEx(hHook);
	return 0;
}