#include "Player.h"

//basic stat
Player::Player(Game* g)
{
	game = g;
	texture = game->LoadTexture("Assets/slime2.png");
	direction = 0;
	pos.x = 0;
	pos.y = 0;
	posRect = { pos.x, pos.y, TILE_SIZE, TILE_SIZE };
	spriteRect = { 0, 0, TILE_SIZE, TILE_SIZE };
}

void Player::Move(int x, int y)
{
	SetDirection(x, y);
	int newPlayerX = pos.x + x;
	int newPlayerY = pos.y + y;

	if (game->HitWall(newPlayerX, newPlayerY))
	{
		return;
	}

	if (!game->BoxUpdated(x, y, newPlayerX, newPlayerY))
	{
		return;
	}

	pos.x = newPlayerX;
	pos.y = newPlayerY;

	posRect.x = pos.x * TILE_SIZE;
	posRect.y = pos.y * TILE_SIZE;
}

void Player::Draw(SDL_Renderer* renderer)
{
	SDL_RenderCopy(renderer, texture, &spriteRect, &posRect);
}

void Player::SetDirection(int x, int y)
{
	if (x > 0)
	{
		direction = 1;
	}
	else if (x < 0)
	{
		direction = 2;
	}
	else if (y > 0)
	{
		direction = 0;
	}
	else if (y < 0)
	{
		direction = 3;
	}

	spriteRect.x = direction * TILE_SIZE;
}

void Player::Reset(int x, int y)
{
	direction = 0;
	pos.x = x;
	pos.y = y;
	posRect = { pos.x * TILE_SIZE, pos.y * TILE_SIZE, TILE_SIZE, TILE_SIZE };
	spriteRect = { 0, 0, TILE_SIZE, TILE_SIZE };
}