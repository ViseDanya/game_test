#include "Renderer.h"
#include "SDL3/SDL.h"

class ColorRenderer : public Renderer
{
    public:
    ColorRenderer();
    ColorRenderer(const SDL_Color& color);
    void render(const aabb& box) const override;
    void setColor(const SDL_Color& color);

    SDL_Color color;
};