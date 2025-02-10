#include "PlayerInputController.h"
#include "constants.h"

void PlayerInputController::ProcessInput(const bool* keystate)
{
    p.velocity.x = 0;
    if (!gravity_enabled || p.is_on_floor)
    {
        p.velocity.y = 0;
    }
    else
    {
        p.velocity.y += GRAVITY * 1.f/FPS;
    }
    if (keystate[left_key])
    {
        p.velocity.x -= PLAYER_SPEED;
    }
    if (keystate[right_key])
    {
        p.velocity.x += PLAYER_SPEED;
    }
    if (keystate[up_key])
    {
        if(!gravity_enabled)
        {
            p.velocity.y += PLAYER_SPEED;
        }
        else if(p.is_on_floor)
        {
            p.velocity.y += PLAYER_JUMP_SPEED;
        }
    }
    if (keystate[down_key] && !gravity_enabled)
    {
        p.velocity.y -= PLAYER_SPEED;
    }
}