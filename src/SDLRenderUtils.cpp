#include "SDL3_image/SDL_image.h"

void renderColoredFilledRectangle(SDL_Renderer* renderer, SDL_Color color, const SDL_FRect& destRect)
{
	SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);
  	SDL_RenderFillRect(renderer, &destRect);
}

void renderColoredRectangle(SDL_Renderer* renderer, SDL_Color color, const SDL_FRect& destRect)
{
	SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);
  	SDL_RenderRect(renderer, &destRect);
}

void renderTexture(SDL_Renderer* renderer, SDL_Texture* texture, const SDL_FRect& sourceRect, const SDL_FRect& destRect)
{
	SDL_RenderTexture(renderer, texture, &sourceRect, &destRect);
}