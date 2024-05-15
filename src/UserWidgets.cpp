#include <windows.h>
#include "include/WidgetText.hpp"
#include "include/BarConfiguration.hpp"
#include <thread>
#include <mutex>
#include <map>
#include <iostream>
#include "include/Global.h"

char *GENERAL_SECTION_W = "Widget1";

std::map<std::string, int> cacheWidgetIntConfig;
std::map<std::string, std::string> cacheWidgetStrConfig;

BOOL WritecfgInt(char *key, int value)
{
	char buffer[10];
	itoa(value, buffer, 10);
	return WritePrivateProfileString(GENERAL_SECTION_W, key, buffer, cfg_FILE_W);
}

BOOL WritecfgColor(char *key, int value)
{

	char buffer[10];
	itoa(value, buffer, 10);
	return WritePrivateProfileString(GENERAL_SECTION_W, key, buffer, cfg_FILE_W);
}

BOOL WritecfgString(const char *key, const char *value)
{
	return WritePrivateProfileString(GENERAL_SECTION_W, key, value, cfg_FILE_W);
}

int GetcfgInt(char *key, int default_value)
{
	std::string keyStr = key;
	keyStr += GENERAL_SECTION_W;
	if (cacheWidgetIntConfig.count(keyStr)) return cacheWidgetIntConfig.at(keyStr);
	else {
		if (debug) std::cout << "[WConfig] " << keyStr << ":int not found, loading disk" << std::endl;
		int gotten = GetPrivateProfileInt(GENERAL_SECTION_W, key, default_value, cfg_FILE_W);
		if(gotten == default_value) WritecfgInt(key, default_value);
		cacheWidgetIntConfig.insert({keyStr, gotten});
		return gotten;
	}
}

int GetcfgColor(char *key, int default_value)
{
	std::string keyStr = key;
	keyStr += GENERAL_SECTION_W;
	if (cacheWidgetIntConfig.count(keyStr)) return cacheWidgetIntConfig.at(keyStr);
	else {
		if (debug) std::cout << "[WConfig] " << keyStr << ":color not found, loading disk" << std::endl;
		int gotten = GetPrivateProfileInt(GENERAL_SECTION_W, key, default_value, cfg_FILE_W);
		if(gotten == default_value) WritecfgColor(key, default_value);
		cacheWidgetIntConfig.insert({keyStr, gotten});
		return gotten;
	}
}

std::string GetcfgString(char *key, const char *default_value)
{
	std::string keyStr = key;
	keyStr += GENERAL_SECTION_W;
	if (cacheWidgetStrConfig.count(keyStr)) return cacheWidgetStrConfig.at(keyStr);
	else {
		if (debug) std::cout << "[WConfig] " << keyStr << ":str not found, loading disk" << std::endl;
		char buffer[256];
		GetPrivateProfileString(GENERAL_SECTION_W, key, default_value, buffer, 256, cfg_FILE_W);
		if(strcmp(buffer, default_value) == 0) WritecfgString(key, default_value);
		std::string result = buffer;
		cacheWidgetStrConfig.insert({keyStr, buffer});
		return result;
	}
}

std::string getActiveWindow() {
	char title[256];
	HWND hwnd = GetForegroundWindow();
	GetWindowText(hwnd, title, sizeof(title));
	return title;
}

void ReloadConfigs() {
	std::cout << "Reloading widget configuration" << std::endl;
	cacheWidgetIntConfig.clear();
	cacheWidgetStrConfig.clear();
}

int widgetCount;

void DrawUserWidgets(HWND hwnd, HDC hdc, RECT rect)
{
	for (int i = 1; i <= widgetCount; i++)
	{
		static char sectionName[50];
		sprintf(sectionName, "Widget%d", i);
		GENERAL_SECTION_W = sectionName;

		int r = GetcfgColor("Red", 255);
		int g = GetcfgColor("Green", 255);
		int b = GetcfgColor("Blue", 255);
		int x = GetcfgInt("X", 0);
		int y = GetcfgInt("Y", 0);
    	std::string type = GetcfgString("Type", "Clock");
		std::string font = GetcfgString("Font", "Arial");

		if (type == "Text")
		{
			std::string text = GetcfgString("Text", "Hello, World!");
			WidgetText textWidget;
			textWidget.SetText(text);
			textWidget.SetFont(font);
			textWidget.SetSize(20);
			textWidget.SetColor(r, g, b);
			textWidget.SetPosition(x, y);
			textWidget.Draw(hdc, rect);
		} else if(type == "ActiveWindow") {
			WidgetText textWidget;
			textWidget.SetText(getActiveWindow());
			textWidget.SetFont(font);
			textWidget.SetSize(20);
			textWidget.SetColor(r, g, b);
			textWidget.SetPosition(x, y);
			textWidget.Draw(hdc, rect);
		} else if(type == "Clock") {
			WidgetText textWidget;
			time_t now = time(0);
			char nowF[100];
			strftime(nowF, 100, "%Y-%m-%d %H:%M:%S", localtime(&now));
			textWidget.SetText(nowF);
			textWidget.SetFont(font);
			textWidget.SetSize(20);
			textWidget.SetColor(r, g, b);
			textWidget.SetPosition(x, y);
			textWidget.Draw(hdc, rect);
		}
	}
}