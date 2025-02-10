#include "aabb.h"
#include "ColorRenderer.h"
#include "constants.h"

static const SDL_Color DEFAULT_COLOR{255,255,255,255};

ColorRenderer::ColorRenderer() : color(DEFAULT_COLOR){};
ColorRenderer::ColorRenderer(const SDL_Color& color) : color(color){};

void ColorRenderer::render(const aabb& box) const
{
    SDL_FRect dest = {box.left(), WINDOW_HEIGHT - box.top(), box.size.x * 2, box.size.y * 2};
	SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);
  	SDL_RenderFillRect(renderer, &dest);
}

void ColorRenderer::setColor(const SDL_Color& color)
{
	this->color = color;
}