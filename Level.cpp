#include "Level.h"

//Load map
void LevelManager::LoadLevel()
{
	int rowCount = 0;
	int colCount = 0;

	string path = "Levels/level" + to_string(currentLevel) + ".txt";

	levelFile.open(path);
	if (levelFile.is_open())
	{
		char inChar;

		while (levelFile.get(inChar))
		{
			if (inChar == '\n')
			{
				colCount = 0;
				rowCount++;
			}
			else
			{
				levelMap[colCount][rowCount] = inChar;
				colCount++;
			}
		}
	}

	levelFile.close();
}

void LevelManager::UpdateLevel()
{
	currentLevel++;
}

void LevelManager::DeUpdateLevel()
{
	currentLevel--;
}