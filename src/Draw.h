#pragma once

#include "SDL3/SDL.h"
#include "aabb.h"

struct SDL_Texture;
struct SDL_Renderer;

struct Draw
{
	SDL_Renderer* renderer;

	Draw(SDL_Renderer* renderer) : renderer(renderer){};
	void renderColoredRectangle(SDL_Color color, const aabb& box);
	void renderTexture(SDL_Texture* texture, const aabb& box);
};

