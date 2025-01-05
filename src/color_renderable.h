#include "renderable.h"
#include "SDL3/SDL.h"

class color_renderable : public renderable
{
    public:
    color_renderable(SDL_Color color);
    void render(aabb box) override;

    SDL_Color color;
};