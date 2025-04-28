#pragma once

#include "Screen.h"

class MainMenuScreen : public Screen
{
    public:
    virtual void render(SDL_Renderer* renderer) override;
};