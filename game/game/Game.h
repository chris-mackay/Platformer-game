#pragma once
#include <SDL.h>
#include <SDL_ttf.h>

struct Vector2
{
	float x;
	float y;
};

class Game
{
public:
	bool Initialize();
	void RunLoop();
	void Shutdown();
private:
	void ProcessInput();
	void UpdateGame();
	void GenerateOutput();
	void DrawText(SDL_Renderer* ren, const char* text, TTF_Font* font, SDL_Color color, int x, int y);
	SDL_Window* window = nullptr;
	SDL_Renderer* renderer = nullptr;
	bool IsRunning = true;
	Vector2 PlayerPos;
	Vector2 PlayerVel;
	Uint32 TicksCount;
	bool CanJump = false;
};