#include "Settings.h"

void Settings::registerAction(int inputKey, const std::string& actionName)
{
	keyMap[inputKey] = actionName;
}

Settings::Difficulty Settings::difficulty = Settings::Difficulty::Normal;
int Settings::musicVolume = 5;
int Settings::sfxVolume = 5;

ActionMap Settings::keyMap;
