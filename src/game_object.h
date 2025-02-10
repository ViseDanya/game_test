#pragma once

#include "aabb.h"
#include "SDL3/SDL.h"

class Renderer;

class game_object
{
public:
    game_object() = default;
    game_object(const aabb& box);

    aabb box;
};