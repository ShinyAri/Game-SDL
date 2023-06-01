#pragma once

#include "Game.h"

class Player
{
public:
	Player(class Game* g);
	void Move(int x, int y);
	void Reset(int x, int y);
	void Draw(SDL_Renderer* renderer);

private:

	//for sprite img
	void SetDirection(int x, int y);
	int direction;

	Vec2 pos;
	SDL_Texture* texture;
	SDL_Rect posRect, spriteRect;
	class Game* game;
};

