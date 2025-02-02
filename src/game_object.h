#pragma once

#include "aabb.h"
#include "SDL3/SDL.h"

class renderable;

class game_object
{
public:
    game_object(aabb box, renderable* game_object_renderable);
    virtual ~game_object();
    void render() const;

    aabb box;
    renderable* game_object_renderable;
};