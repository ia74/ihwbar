#ifndef BAR_CONFIGURATION_HPP
#define BAR_CONFIGURATION_HPP

#include <windows.h>

// Declare your functions here
int GetConfigInt(const char* key, int default_value);
int GetConfigColor(const char* key, int default_value);
const char* GetConfigString(const char* key, const char* default_value);
BOOL WriteConfigInt(const char* key, int value);
BOOL WriteConfigColor(const char* key, int value);
BOOL WriteConfigString(const char* key, const char* value);
static char* CONFIG_FILE = ".\\config.ihw";
static char* GENERAL_SECTION = "IHWBar";

#endif // BAR_CONFIGURATION_HPP