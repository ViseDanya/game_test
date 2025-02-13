#include "Draw.h"
#include "constants.h"
#include "SDL3_image/SDL_image.h"

void Draw::renderColoredRectangle(SDL_Color color, const aabb& box)
{
    SDL_FRect dest = {box.left(), WINDOW_HEIGHT - box.top(), 
                        box.size.x * 2, box.size.y * 2};
	SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);
  	SDL_RenderFillRect(renderer, &dest);
}

void Draw::renderTexture(SDL_Texture* texture, const aabb& box)
{
    SDL_FRect dest = {box.left(), WINDOW_HEIGHT - box.top(), 
                        box.size.x * 2, box.size.y * 2};
	SDL_RenderTexture(renderer, texture, nullptr, &dest);
}