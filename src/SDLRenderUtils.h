#pragma once

#include "SDL3/SDL.h"

struct SDL_Texture;

void renderColoredFilledRectangle(SDL_Renderer* renderer, SDL_Color color, const SDL_FRect& destRect);
void renderColoredRectangle(SDL_Renderer* renderer, SDL_Color color, const SDL_FRect& destRect);
void renderTexture(SDL_Renderer* renderer, SDL_Texture* texture, const SDL_FRect& sourceRect, const SDL_FRect& destRect);

