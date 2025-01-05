#include "player.h"
#include "constants.h"
#include "color_renderable.h"
#include <iostream>

player::player(vec2 position) : dynamic_object(aabb(position, vec2(PLAYER_WIDTH / 2, PLAYER_HEIGHT / 2)), new color_renderable(PLAYER_COLOR)) {};

void player::process_input(const bool *keystate, const bool gravity_enabled)
{
    velocity.x = 0;
    if (!gravity_enabled || is_on_floor)
    {
        velocity.y = 0;
    }
    else
    {
        velocity.y += GRAVITY * 1.f/FPS;
    }
    if (keystate[left_key])
    {
        velocity.x -= PLAYER_SPEED;
    }
    if (keystate[right_key])
    {
        velocity.x += PLAYER_SPEED;
    }
    if (keystate[up_key])
    {
        if(!gravity_enabled)
        {
            velocity.y += PLAYER_SPEED;
        }
        else if(is_on_floor)
        {
            velocity.y += PLAYER_JUMP_SPEED;
        }
    }
    if (keystate[down_key] && !gravity_enabled)
    {
        velocity.y -= PLAYER_SPEED;
    }
}