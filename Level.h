#pragma once

#include "Utils.h"
#include <fstream>
#include <string>

class LevelManager
{
public:
	void LoadLevel();
	void UpdateLevel();
	void DeUpdateLevel();
	char levelMap[TILE_ROWS][TILE_COLS] = { '0' };

private:

	//Read input from file
	ifstream levelFile;

	int currentLevel = 1;
};

