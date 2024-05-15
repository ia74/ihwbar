#include <iostream>
#include <fstream>
#include "BarConfiguration.hpp"
#include "Global.h"
namespace Boot
{
	bool Init(
		int &r,
		int &g,
		int &b,
		int &h,
		int &sc,
		int &tcr,
		int &tcg,
		int &tcb,
		int &bx,
		int &by,
		int &bw
	) {
		r = GetConfigColor("Red", 0);
		g = GetConfigColor("Green", 0);
		b = GetConfigColor("Blue", 255);
		h = GetConfigInt("Bar Height", 25);
		sc = GetConfigInt("ShowConsole", 1);
		tcr = GetConfigColor("Text Red", 0);
		tcg = GetConfigColor("Text Green", 0);
		tcb = GetConfigColor("Text Blue", 0);
		bx = GetConfigInt("Bar X", 0);
		by = GetConfigInt("Bar Y", 0);
		bw = GetConfigInt("Bar Width Offset", 0);
		WriteConfigColor("Red", r);
		WriteConfigColor("Green", g);
		WriteConfigColor("Blue", b);
		WriteConfigInt("Bar Height", h);
		WriteConfigInt("ShowConsole", sc);
		WriteConfigColor("Text Red", tcr);
		WriteConfigColor("Text Green", tcg);
		WriteConfigColor("Text Blue", tcb);
		WriteConfigInt("Bar X", bx);
		WriteConfigInt("Bar Y", by);
		WriteConfigInt("Bar Width Offset", bw);
		paint_r = r;
		paint_g = g;
		paint_b = b;
		paint_tr = tcr;
		paint_tg = tcg;
		paint_tb = tcb;
		widgetCount = GetConfigInt("WidgetCount", 1);
		WriteConfigInt("WidgetCount", widgetCount);
		if (debug)
		{
			std::cout << "Red: " << r << std::endl;
			std::cout << "Green: " << g << std::endl;
			std::cout << "Blue: " << b << std::endl;
			std::cout << "Height: " << h << std::endl;
			std::cout << "ShowConsole: " << sc << std::endl;
			std::cout << "Text Red: " << tcr << std::endl;
			std::cout << "Text Green: " << tcg << std::endl;
			std::cout << "Text Blue: " << tcb << std::endl;
			std::cout << "Bar X: " << bx << std::endl;
			std::cout << "Bar Y: " << by << std::endl;
			std::cout << "Bar Width Offset: " << bw << std::endl;
			std::cout << "WidgetCount: " << widgetCount << std::endl;
		}
		return true;
	}

	void Welcome() {
		std::cout << "Bar config loaded from " << CONFIG_FILE << "." << std::endl;
		std::cout << "Widget config loaded from " << cfg_FILE_W << "." << std::endl;
		std::cout << "Welcome to IHWBar!" << std::endl << std::endl;
		std::cout << "Keybinds:" << std::endl;
		std::cout << "ESC: Quit" << std::endl;
		std::cout << "F1: Open Config Editor" << std::endl;
		std::cout << "R: Reload bar height" << std::endl;
		std::cout << "Win+CTRL+H: Hide/Show bar" << std::endl;
		std::cout << "Win+CTRL+R: Reload bar (from anywhere)" << std::endl;
		std::cout << "Win+CTRL+ESC: Exit bar (from anywhere)" << std::endl;
	}
}
