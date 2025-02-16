#pragma once

#include "SDL3/SDL.h"
#include "Box.h"

struct SDL_Texture;
struct SDL_Renderer;

struct Renderer
{
	SDL_Renderer* renderer;

	Renderer(SDL_Renderer* renderer) : renderer(renderer){};
	void renderColoredRectangle(SDL_Color color, const Box& box);
	void renderTexture(SDL_Texture* texture, const Box& box);
};

