#pragma once

#include "Constants.h"
#include "Direction.h"

struct Trampoline
{
    enum State
    {
        IDLE, TRIGGERED
    };

    State state = IDLE;
    float impulse = PLAYER_JUMP_SPEED;
};