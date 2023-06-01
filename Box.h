#pragma once

#include "Utils.h"

class Box
{
public:
	Box(int x, int y);
	~Box();

	//allow to move box
	void Update(int x, int y, bool complete);
	Vec2 GetPos();
	SDL_Rect* GetRect();

	//box in goal or not
	bool GetInGoal();

private:
	Vec2 pos;
	SDL_Rect rect;
	bool inGoal;
};

