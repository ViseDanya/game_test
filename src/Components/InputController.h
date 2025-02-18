#pragma once

#include <SDL3/SDL.h>

struct InputController
{
    SDL_Scancode left_key;
    SDL_Scancode right_key;
    SDL_Scancode up_key;
    SDL_Scancode down_key;
    bool gravity_enabled;
};