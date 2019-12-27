#include <iostream>
#include <ctime>
#include <SDL.h>
#include <SDL_ttf.h>
#include "Game.h"
#include <string>
#include <fstream>
#include <vector>
#include <sstream>

using namespace std;

const int thickness = 15;
float GRAVITY = 20.0f;
float duration;
vector<SDL_Rect> platforms;
int level = 1;
string file = "";

void LoadPlatforms(SDL_Renderer* ren)
{

	file = "level" + to_string(level) + ".csv";

	ifstream in(file);
	string line, field;
	vector<vector<string>> array; // 2D array
	vector<string> v; // array of values for one line only
	platforms.clear();

	while (getline(in, line)) // get next line in file
	{
		v.clear();
		stringstream ss(line);

		while (getline(ss, field, ',')) // break line into comma delimitted fields
		{
			v.push_back(field); // add each field to the 1D array
		}
		array.push_back(v); // add the 1D array to the 2D array
	}

	for (int i = 0; i < array.size(); ++i)
	{
		for (int j = 0; j < 8; ++j)
		{
			if (array[i][j] == "0") // 0 represents a platform in the csv file
			{
				int l = 100; // platform width
				int h = 15; // platform height

				// 8 columns * 100 (platform width) = 800 screen width
				int x = ((j + 1) * 100) - 100;

				// 40 rows * 15 (platform height) = 600 screen height
				int y = ((i + 1) * 15) - 15;

				SDL_Rect platform{ x, y, l, h };
				platforms.push_back(platform); // add platform to the 1D array for collision testing
				SDL_RenderFillRect(ren, &platform);
			}
		}
	}
}

inline const char* const BoolToString(bool b)
{
	return b ? "true" : "false";
}

bool Game::Initialize()
{
	int result = SDL_Init(SDL_INIT_EVERYTHING);

	if (TTF_Init() != 0)
	{
		SDL_Log("Unable to initialize SDL_ttf: %s", SDL_GetError());
	}

	if (result != 0)
	{
		SDL_Log("Unable to initialize SDL: %s", SDL_GetError());
		return false;
	}

	window = SDL_CreateWindow("Platformer game", 100, 100, 800, 600, 0);

	if (!window)
	{
		SDL_Log("Unable to create window: %s", SDL_GetError());
		return false;
	}

	renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);

	if (!renderer)
	{
		SDL_Log("Unable to create renderer: %s", SDL_GetError());
		return false;
	}

	PlayerPos.x = 800.0f / 2.0f;
	PlayerPos.y = 600 - thickness - (thickness / 2);
	PlayerVel.x = 0.0f;
	PlayerVel.y = 0.0f;

	return true;
}

void Game::Shutdown()
{
	SDL_DestroyWindow(window);
	SDL_DestroyRenderer(renderer);
	SDL_Quit();
	TTF_Quit();
}

void Game::ProcessInput()
{
	SDL_Event event;
	while (SDL_PollEvent(&event))
	{
		switch (event.type)
		{
		case SDL_QUIT:
			IsRunning = false;
			break;
		case SDL_KEYDOWN:
			switch (event.key.keysym.sym)
			{
			case SDLK_LEFT:
				PlayerVel.x = -250.0f;;
				break;
			case SDLK_RIGHT:
				PlayerVel.x = 250.0f;;
				break;
			default:
				break;
			}
		case SDL_KEYUP:
			PlayerVel.x = 0.0f;
			break;
		default:
			break;
		}

		const Uint8* state = SDL_GetKeyboardState(NULL);

		if (state[SDL_SCANCODE_ESCAPE])
			IsRunning = false;

		if (state[SDL_SCANCODE_RETURN])
		{
			level += 1;
		}

		if (state[SDL_SCANCODE_SPACE])
		{
			if (CanJump)
			{
				clock_t start = clock();
				duration = (clock() - start) / (float)CLOCKS_PER_SEC;
				PlayerVel.y = -450.0f;
				CanJump = false;
			}
		}

		if (state[SDL_SCANCODE_LEFT])
			PlayerVel.x = -250.0f;

		if (state[SDL_SCANCODE_RIGHT])
			PlayerVel.x = +250.0f;
	}
}

void Game::UpdateGame()
{
	// Wait until 16ms has elapsed since last frame
	while (!SDL_TICKS_PASSED(SDL_GetTicks(), TicksCount + 16));

	float deltaTime = (SDL_GetTicks() - TicksCount) / 1000.0f;
	TicksCount = SDL_GetTicks();

	if (deltaTime > 0.15f)
		deltaTime = 0.15f; // Clamp the maximum delta time value

	TicksCount = SDL_GetTicks(); // Update tick count (for next frame)

	bool flag = false;

	for (SDL_Rect platform : platforms)
	{
		// Check if the player hit the left side of the platform
		if (PlayerPos.x + thickness > platform.x
			&& PlayerPos.x + thickness < (platform.x + 3)
			&& PlayerPos.y + thickness > platform.y
			&& PlayerPos.y < platform.y + platform.h
			&& PlayerVel.x > 0.0f)
		{
			PlayerVel.x = 0.0f;
			PlayerPos.x = platform.x - thickness;
		}

		// Check if the player hit the right side of the platform
		if (PlayerPos.x < platform.x + platform.w
			&& PlayerPos.x > platform.x + (platform.w - 3)
			&& PlayerPos.y + thickness > platform.y
			&& PlayerPos.y < platform.y + platform.h
			&& PlayerVel.x < 0.0f)
		{
			PlayerVel.x = 0.0f;
			PlayerPos.x = platform.x + platform.w;
		}

		if (!flag) // this breaks out of checking if the player is already on a platform
		{
			// Check if the player is on the platform
			if (PlayerPos.y + thickness > platform.y
				&& PlayerPos.y + thickness < platform.y + platform.h
				&& PlayerPos.x < platform.x + platform.w
				&& PlayerPos.x + thickness > platform.x
				&& PlayerVel.y > 0.0f)
			{
				CanJump = true;
				flag = true;
				PlayerVel.y = 0.0f;
				PlayerPos.y = platform.y - thickness;
			}
			else
				CanJump = false;
		}

		// Check if the player hit the bottom of the platform
		if (PlayerPos.x + thickness > platform.x
			&& PlayerPos.x < platform.x + platform.w
			&& PlayerPos.y < platform.y + platform.h
			&& PlayerPos.y > platform.y + (platform.h / 2)
			&& CanJump == false
			&& PlayerVel.y < 0.0f)
		{
			PlayerVel.y = 0.0f;
			PlayerPos.y = platform.y + platform.h;
		}
	}

	PlayerPos.x += PlayerVel.x * deltaTime;
	PlayerVel.y += GRAVITY;
	PlayerPos.y += PlayerVel.y * deltaTime;

	if (duration == 3.5)
		PlayerVel.y = 0.0f;
}

void Game::GenerateOutput()
{
	SDL_SetRenderDrawColor(renderer, 0, 24, 0, 255);
	SDL_RenderClear(renderer);
	SDL_SetRenderDrawColor(renderer, 45, 245, 41, 255);

	SDL_Rect player
	{
		static_cast<int>(PlayerPos.x),
		static_cast<int>(PlayerPos.y),
		thickness,
		thickness
	};

	TTF_Font* font = TTF_OpenFont("visitor1.ttf", 25);
	SDL_Color color = { 255, 255, 255 };

	string can_jump = "CanJump | " + (string)BoolToString(CanJump);
	string x = "Pos.x | " + to_string(round(PlayerPos.x));
	string y = "Pos.y | " + to_string(round(PlayerPos.y));
	string vel_x = "Vel.x | " + to_string(round(PlayerVel.x));
	string vel_y = "Vel.y | " + to_string(round(PlayerVel.y));

	DrawText(renderer, can_jump.c_str(), font, color, 10, 10);
	DrawText(renderer, x.c_str(), font, color, 10, 40);
	DrawText(renderer, y.c_str(), font, color, 10, 70);
	DrawText(renderer, vel_x.c_str(), font, color, 10, 130);
	DrawText(renderer, vel_y.c_str(), font, color, 10, 160);
	DrawText(renderer, file.c_str(), font, color, 650, 10);

	LoadPlatforms(renderer);
	SDL_RenderFillRect(renderer, &player);
	SDL_RenderPresent(renderer);
}

void Game::DrawText(SDL_Renderer* ren, const char* text, TTF_Font* font, SDL_Color color, int x, int y)
{
	SDL_Surface* surface = TTF_RenderText_Solid(font, text, color);
	SDL_Texture* texture = SDL_CreateTextureFromSurface(ren, surface);
	SDL_FreeSurface(surface);

	int texW = 0;
	int texH = 0;

	SDL_QueryTexture(texture, NULL, NULL, &texW, &texH);
	SDL_Rect rect = { x, y, texW, texH };
	SDL_RenderCopy(ren, texture, NULL, &rect);
}

void Game::RunLoop()
{
	while (IsRunning)
	{
		ProcessInput();
		UpdateGame();
		GenerateOutput();
	}
}