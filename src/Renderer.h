#pragma once

#include "SDL3/SDL.h"
#include "Box.h"

struct SDL_Texture;
struct SDL_Renderer;

struct Renderer
{
	SDL_Renderer* renderer;

	Renderer(SDL_Renderer* renderer) : renderer(renderer){};
	void renderColoredFilledRectangle(SDL_Color color, const SDL_FRect& destRect);
	void renderColoredRectangle(SDL_Color color, const SDL_FRect& destRect);
	void renderTexture(SDL_Texture* texture, const SDL_FRect& sourceRect, const SDL_FRect& destRect);
};

