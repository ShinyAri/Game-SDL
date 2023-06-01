#include "Game.h"

bool Game::Init()
{
	if (SDL_Init(SDL_INIT_EVERYTHING) != 0)
	{
		cout << "SDL failed to initialize: " << SDL_GetError() << endl;
		return false;
	}

	if (IMG_Init(IMG_INIT_PNG) == 0)
	{
		cout << "SDL_Image failed to initialize: " << SDL_GetError() << endl;
		return false;
	}

	window = SDL_CreateWindow("Slimekoban", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, false);
	if (!window)
	{
		cout << "Window failed to initialize: " << SDL_GetError() << endl;
		return false;
	}

	renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
	if (!renderer)
	{
		cout << "Renderer failed to initialize: " << SDL_GetError() << endl;
		return false;
	}

	if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0)
	{
		printf("SDL_mixer could not initialize! SDL_mixer Error: %s\n", Mix_GetError());
		return false;
	}

	levelManager = new LevelManager();
	levelManager->LoadLevel();

	wallTexture = LoadTexture("Assets/wall2.png");
	groundTexture = LoadTexture("Assets/ground3.png");
	boxTexture = LoadTexture("Assets/hop4.png");
	goalTexture = LoadTexture("Assets/goal2.png");
	darkTexture = LoadTexture("Assets/dark.png");

	gMusic = Mix_LoadMUS("Music/rv.mp3");
	
	player = new Player(this);

	InitLevel();

	return true;
}

void Game::GameLoop()
{
	while (isRunning)
	{
		if (Mix_PlayingMusic() == 0)
		{
			Mix_PlayMusic(gMusic, -1);
		}
		HandleEvents();
		Update();
		Draw();
	}
}

void Game::HandleEvents()
{
	SDL_Event event;

	while (SDL_PollEvent(&event) != 0)
	{
		if (event.type == SDL_QUIT)
		{
			isRunning = false;
		}
		if (event.type == SDL_KEYDOWN)
		{
			switch (event.key.keysym.sym)	
			{
			case SDLK_RIGHT:
				player->Move(1, 0);
				break;
			case SDLK_LEFT:
				player->Move(-1, 0);
				break;
			case SDLK_DOWN:
				player->Move(0, 1);
				break;
			case SDLK_UP:
				player->Move(0, -1);	
				break;
			case SDLK_r:
				DestroyBoxes();
				InitLevel();
				break;
			case SDLK_e:
				GoToNextLevel();
				break;
			case SDLK_w:
				GoToPrevLevel();
				break;
			case SDLK_m:
				//If the music is paused
				if (Mix_PausedMusic() == 1)
				{
					//Resume the music
					Mix_ResumeMusic();
				}
				//If the music is playing
				else
				{
					//Pause the music
					Mix_PauseMusic();
				}
				break;
			default:
				break;
			}
		}
	}

	const Uint8* keystates = SDL_GetKeyboardState(NULL);

	if (keystates[SDL_SCANCODE_ESCAPE])
	{
		isRunning = false;
	}
}

void Game::Update()
{

}

void Game::Draw()
{
	for (int r = 0; r < TILE_ROWS; r++)
	{
		for (int c = 0; c < TILE_COLS; c++)
		{
			SDL_Rect rect = { c * TILE_SIZE, r * TILE_SIZE, TILE_SIZE, TILE_SIZE };

			if (levelManager->levelMap[c][r] == 'x')
			{
				SDL_RenderCopy(renderer, wallTexture, NULL, &rect);
			}
			else if (levelManager->levelMap[c][r] == 'g')
			{
				SDL_RenderCopy(renderer, goalTexture, NULL, &rect);
			}
			else if (levelManager->levelMap[c][r] == 'd')
			{
				SDL_RenderCopy(renderer, darkTexture, NULL, &rect);
			}
			else
			{
				SDL_RenderCopy(renderer, groundTexture, NULL, &rect);
			}
		}
	}

	for (int i = 0; i < boxes.size(); i++)
	{
		SDL_RenderCopy(renderer, boxTexture, NULL, boxes[i]->GetRect());
	}

	player->Draw(renderer);

	SDL_RenderPresent(renderer);
}

void Game::Shutdown()
{
	SDL_DestroyTexture(wallTexture);
	SDL_DestroyTexture(groundTexture);
	SDL_DestroyTexture(boxTexture);
	SDL_DestroyTexture(goalTexture);
	SDL_DestroyTexture(darkTexture);

	Mix_FreeMusic(gMusic);
	gMusic = NULL;

	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	window = NULL;
	renderer = NULL;

	IMG_Quit();
	SDL_Quit();
	Mix_Quit();
}

SDL_Texture* Game::LoadTexture(string path)
{
	SDL_Surface* tempSurface = IMG_Load(path.c_str());
	if (tempSurface == NULL)
	{
		cout << "Failed to load surface: " << IMG_GetError() << endl;
	}

	SDL_Texture* newTexture = SDL_CreateTextureFromSurface(renderer, tempSurface);
	if (newTexture == NULL)
	{
		cout << "Failed to conver to texture: " << SDL_GetError() << endl;
	}

	SDL_FreeSurface(tempSurface);

	return newTexture;
}

bool Game::HitWall(int x, int y)
{
	return levelManager->levelMap[x][y] == 'x';
}

bool Game::CanPushBox(Box* box, int x, int y)
{
	// Exit if trying to push into wall
	if (HitWall(x, y))
	{
		return false;
	}

	// Exit if trying to push into another box
	for (int i = 0; i < boxes.size(); i++)
	{
		if (boxes[i] == box)
		{
			continue;
		}
		else if (x == boxes[i]->GetPos().x && y == boxes[i]->GetPos().y)
		{
			return false;
		}
	}

	return true;
}

bool Game::BoxUpdated(int moveX, int moveY, int pX, int pY)
{
	Box* boxToPush = nullptr;

	// Find box attempting to push
	for (int i = 0; i < boxes.size(); i++)
	{
		if (pX == boxes[i]->GetPos().x && pY == boxes[i]->GetPos().y)
		{
			boxToPush = boxes[i];
		}
	}

	// Check if can push the box -> update the box
	if (boxToPush != nullptr)
	{
		int toPushX = pX + moveX;
		int toPushY = pY + moveY;

		if (CanPushBox(boxToPush, toPushX, toPushY))
		{
			bool inGoal = HitGoal(toPushX, toPushY);
			boxToPush->Update(toPushX, toPushY, inGoal);
			if (AllGoalsComplete())
			{
				GoToNextLevel();
				return false;
			}
		}
		else
		{
			return false;
		}
	}

	return true;
}

bool Game::HitGoal(int x, int y)
{
	return levelManager->levelMap[x][y] == 'g';
}

bool Game::AllGoalsComplete()
{
	for (int i = 0; i < boxes.size(); i++)
	{
		if (!boxes[i]->GetInGoal())
		{
			return false;
		}
	}

	return true;
}

void Game::DestroyBoxes()
{
	for (int i = 0; i < boxes.size(); i++)
	{
		delete boxes[i];
	}

	boxes.erase(boxes.begin(), boxes.end());
}

void Game::InitLevel()
{
	// Reset Player and add new boxes
	for (int r = 0; r < TILE_ROWS; r++)
	{
		for (int c = 0; c < TILE_COLS; c++)
		{
			if (levelManager->levelMap[c][r] == 'p')
			{
				player->Reset(c, r);
			}
			else if (levelManager->levelMap[c][r] == 'b')
			{
				boxes.emplace_back(new Box(c, r));
			}
		}
	}
}

void Game::GoToNextLevel()
{
	DestroyBoxes();

	// Go to next level
	levelManager->UpdateLevel();
	levelManager->LoadLevel();

	InitLevel();
}

void Game::GoToPrevLevel()
{
	DestroyBoxes();

	// Go to next level
	levelManager->DeUpdateLevel();
	levelManager->LoadLevel();

	InitLevel();
}