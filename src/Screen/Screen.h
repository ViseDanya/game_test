#pragma once

#include <SDL3/SDL.h>

class Screen
{
    public:
    virtual void handleEvents(const SDL_Event& event);
    virtual void update(const float dt){};
    virtual void render(SDL_Renderer* renderer){};
};