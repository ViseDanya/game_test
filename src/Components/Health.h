#pragma once

#include <SDL3/SDL.h>
#include <algorithm>

struct Health
{
    enum State
    {
        IDLE, DAMAGED
    };

    int health = 10;
    int minHealth = 0;
    int maxHealth = health;
    State state;
    Uint64 damageTime;
    Uint64 damageDuration = 2000;

    void applyChangeToHealth(int amount)
    {
        int prevHealth = health;
        health += amount;
        health = std::clamp(health, minHealth, maxHealth);
        if(health < prevHealth && state == IDLE)
        {
            state = DAMAGED;
            damageTime = SDL_GetTicks();
        }
    }
};