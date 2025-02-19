#pragma once

#include <entt/entt.hpp>
#include <InputController.h>
#include <Velocity.h>
#include <Adjacencies.h>

void applyInputToVelocity(entt::registry& registry, const bool* keystate, const bool gravity_enabled)
{
    auto view = registry.view<Velocity, const InputController, const Adjacencies>();
    view.each([&](Velocity& velocity, const InputController& inputController, const Adjacencies& adjacencies) 
        {
            if (keystate[inputController.left_key])
            {
                velocity.velocity.x -= PLAYER_SPEED * 1.f/FPS;
            }
            if (keystate[inputController.right_key])
            {
                velocity.velocity.x += PLAYER_SPEED * 1.f/FPS;
            }
            if (keystate[inputController.up_key])
            {
              if(!gravity_enabled)
              {
                velocity.velocity.y += PLAYER_SPEED * 1.f/FPS;
              }
              else if(adjacencies.isOnFloor)
              {
                velocity.velocity.y += PLAYER_JUMP_SPEED * 1.f/FPS;
              }
            }
            if (keystate[inputController.down_key] && !gravity_enabled)
            {
                velocity.velocity.y -= PLAYER_SPEED * 1.f/FPS;
            }
    });
};