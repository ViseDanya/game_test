#include "aabb.h"
#include "color_renderable.h"
#include "constants.h"


color_renderable::color_renderable(SDL_Color color) : color(color){};

void color_renderable::render(aabb box)
{
    SDL_FRect dest = {box.left(), WINDOW_HEIGHT - box.top(), box.size.x * 2, box.size.y * 2};
	SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);
  	SDL_RenderFillRect(renderer, &dest);
}