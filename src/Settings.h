#pragma once

#include "Scene.h"

namespace Settings
{
	enum class Difficulty
	{
		Easy,
		Normal,
		Hard
	};

	extern Difficulty difficulty;
	extern int sfxVolume;
	extern int musicVolume;
	extern ActionMap keyMap;

	void registerAction(int inputKey, const std::string& actionName);
}