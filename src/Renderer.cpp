#include "Renderer.h"
#include "Constants.h"
#include "SDL3_image/SDL_image.h"

void Renderer::renderColoredFilledRectangle(SDL_Color color, const SDL_FRect& destRect)
{
	SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);
  	SDL_RenderFillRect(renderer, &destRect);
}

void Renderer::renderColoredRectangle(SDL_Color color, const SDL_FRect& destRect)
{
	SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);
  	SDL_RenderRect(renderer, &destRect);
}

void Renderer::renderTexture(SDL_Texture* texture, const SDL_FRect& sourceRect, const SDL_FRect& destRect)
{
	SDL_RenderTexture(renderer, texture, &sourceRect, &destRect);
}