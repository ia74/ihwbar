#include <windows.h>

#include "bar.h"
#include "BarConfiguration.hpp"

LRESULT CALLBACK DialogProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	char buffer[10];
	int ra, ga, ba, UserHeighta;
	switch (msg)
	{
	case WM_INITDIALOG:
		ra = GetConfigColor("Red", 255);
		ga = GetConfigColor("Green", 255);
		ba = GetConfigColor("Blue", 255);
		UserHeighta = GetConfigInt("Bar Height", 100);
		char buffer[10];
		itoa(ra, buffer, 10);
		SetDlgItemText(hwnd, IDC_EDIT1, buffer);
		itoa(ga, buffer, 10);
		SetDlgItemText(hwnd, IDC_EDIT2, buffer);
		itoa(ba, buffer, 10);
		SetDlgItemText(hwnd, IDC_EDIT3, buffer);
		itoa(UserHeighta, buffer, 10);
		SetDlgItemText(hwnd, IDC_EDIT4, buffer);
		return TRUE;
	case WM_COMMAND:
		switch (LOWORD(wParam))
		{
		case IDOK:
			GetDlgItemText(hwnd, IDC_EDIT1, buffer, 10);
			ra = atoi(buffer);
			GetDlgItemText(hwnd, IDC_EDIT2, buffer, 10);
			ga = atoi(buffer);
			GetDlgItemText(hwnd, IDC_EDIT3, buffer, 10);
			ba = atoi(buffer);
			GetDlgItemText(hwnd, IDC_EDIT4, buffer, 10);
			UserHeighta = atoi(buffer);
			WriteConfigColor("Red", ra);
			WriteConfigColor("Green", ga);
			WriteConfigColor("Blue", ba);
			WriteConfigInt("Bar Height", UserHeighta);
			EndDialog(hwnd, IDOK);
			break;
		case IDCANCEL:
			EndDialog(hwnd, IDCANCEL);
			break;
		}
		return 0;
	}
	return 0;
}