#pragma once

#include <Direction.h>

struct Trampoline
{
    float impulse = PLAYER_JUMP_SPEED;
    bool isTriggered = false;
};