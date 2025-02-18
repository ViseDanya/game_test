#include <SDL3/SDL.h>
#include <SDL3_image/SDL_image.h>

struct Sprite
{
    SDL_Texture* texture;
    SDL_FRect sourceRect;
};