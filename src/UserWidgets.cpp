#include <windows.h>
#include "widgets/WidgetText.hpp"
#include "BarConfiguration.hpp"
#include <thread>
#include <mutex>
#include <map>

char *cfg_FILE_W = ".\\widgets.ihw";
char *GENERAL_SECTION_W = "Widget1";

std::map<std::string, std::string> config;
std::mutex configMutex;

void WriteConfigToMemory(const std::string& key, const std::string& value) {
    std::lock_guard<std::mutex> lock(configMutex);
    config[key] = value;
}


int GetcfgInt(char *key, int default_value)
{
	return GetPrivateProfileInt(GENERAL_SECTION_W, key, default_value, cfg_FILE_W);
}

int GetcfgColor(char *key, int default_value)
{
	return GetPrivateProfileInt("Color", key, default_value, cfg_FILE_W);
}

const char *GetcfgString(char *key, const char *default_value)
{
	static char buffer[256];
	GetPrivateProfileString(GENERAL_SECTION_W, key, default_value, buffer, 256, cfg_FILE_W);
	return buffer;
}

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
	return WritePrivateProfileString("Color", key, buffer, cfg_FILE_W);
}

BOOL WritecfgString(const char *key, const char *value)
{
	return WritePrivateProfileString(GENERAL_SECTION_W, key, value, cfg_FILE_W);
}

void SaveConfigToDisk() {
    std::lock_guard<std::mutex> lock(configMutex);
    for (const auto& pair : config) {
        // Replace this with your actual function to write to disk
        WritecfgString(pair.first.c_str(), pair.second.c_str());
    }
}

std::string getActiveWindow() {
	char title[256];
	HWND hwnd = GetForegroundWindow();
	GetWindowText(hwnd, title, sizeof(title));
	return title;
}

void DrawUserWidgets(HWND hwnd, HDC hdc, RECT rect)
{
	int widgetCount = GetConfigInt("WidgetCount", 1);
	for (int i = 1; i <= widgetCount; i++)
	{
		static char sectionName[50];
		sprintf(sectionName, "Widget%d", i);
		GENERAL_SECTION_W = sectionName;

		int r, g, b;
		r = GetcfgInt("Red", 0);
		g = GetcfgInt("Green", 0);
		b = GetcfgInt("Blue", 0);
		int x, y;
		x = GetcfgInt("X", 0);
		y = GetcfgInt("Y", 0);
		const char *type = GetcfgString("Type", "Text");

		std::thread t1([r,g,b,x,y,type]() {
			WriteConfigToMemory("Red", std::to_string(r));
			WriteConfigToMemory("Green", std::to_string(g));
			WriteConfigToMemory("Blue", std::to_string(b));
			WriteConfigToMemory("X", std::to_string(x));
			WriteConfigToMemory("Y", std::to_string(y));
			WriteConfigToMemory("Type", type);
		});

		t1.join();

		if (strcmp(type, "Text") == 0)
		{
			const char *text = GetcfgString("Text", "Hello, World!");
			WidgetText textWidget;
			textWidget.SetText(text);
			textWidget.SetFont("Arial");
			textWidget.SetSize(20);
			textWidget.SetColor(r, g, b);
			textWidget.SetPosition(x, y);
			textWidget.Draw(hdc, rect);
		} else if(strcmp(type, "ActiveWindow") == 0) {
			WidgetText textWidget;
			textWidget.SetText(getActiveWindow());
			textWidget.SetFont("Arial");
			textWidget.SetSize(20);
			textWidget.SetColor(r, g, b);
			textWidget.SetPosition(x, y);
			textWidget.Draw(hdc, rect);
		} else if(strcmp(type, "Clock") == 0) {
			WidgetText textWidget;
			time_t now = time(0);
			char nowF[100];
			strftime(nowF, 100, "%Y-%m-%d %H:%M:%S", localtime(&now));
			textWidget.SetText(nowF);
			textWidget.SetFont("Arial");
			textWidget.SetSize(20);
			textWidget.SetColor(r, g, b);
			textWidget.SetPosition(x, y);
			textWidget.Draw(hdc, rect);
		}
	}
}

std::thread t2([]() {
    while (true) {
        std::this_thread::sleep_for(std::chrono::seconds(10));
        SaveConfigToDisk();
    }
});