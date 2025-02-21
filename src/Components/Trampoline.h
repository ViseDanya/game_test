#pragma once

#include <Direction.h>

struct Trampoline
{
    float impulse = 2*PLAYER_JUMP_SPEED;
    bool isTriggered = false;
};