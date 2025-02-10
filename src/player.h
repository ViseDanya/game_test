#pragma once

#include "dynamic_object.h"
#include "SDL3/SDL.h"

const int PLAYER_WIDTH = 32;
const int PLAYER_HEIGHT = 32;

class Player : public dynamic_object
{
    public:
    Player() = default;
    Player(const vec2& position);
    virtual ~Player() = default;
};