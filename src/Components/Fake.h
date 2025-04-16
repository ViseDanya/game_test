#pragma once

#include <SDL3/SDL.h>

struct Fake
{
    enum State
    {
        IDLE, TRIGGERED, FLIP, FLIPPING
    };
    
    Uint64 collisionTime;
    Uint64 timeUntilFlipMS = 100;
    Uint64 flipDuration = 1000;
    Fake::State state = IDLE;
};