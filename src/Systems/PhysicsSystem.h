#pragma once

#include "Components/Box.h"
#include "Components/Velocity.h"
#include "Components/Adjacencies.h"
#include <entt/entt.hpp>

void resetVelocity(entt::registry& registry, const bool gravityEnabled)
{
  auto view = registry.view<Velocity, const Adjacencies>();
  view.each([&](entt::entity e, Velocity& velocity, const Adjacencies& adjacencies) 
  {
    velocity.velocity.x = 0;
    if(adjacencies.isOnFloor || !gravityEnabled)
    {
      velocity.velocity.y = 0;
    }
    if(gravityEnabled)
    {
      velocity.velocity.y += GRAVITY * 1.f/FPS * 1.f/FPS;
    }
  });
}


void applyVelocityToPosition(entt::registry& registry)
{
    auto view = registry.view<const Velocity, Box>();
    view.each([&](const Velocity& velocity, Box& box) 
        {
            box.center += velocity.velocity;
        });
}

void resetVerticalVelocityForEntitiesOnFloor(entt::registry& registry)
{
  auto view = registry.view<Velocity, const Adjacencies>();
  view.each([&](Velocity& velocity, const Adjacencies& adjacencies) 
  {
    if(adjacencies.isOnFloor)
    {
      velocity.velocity.y = 0;
    }
  });
}

void resetAdjacencies(entt::registry& registry)
{
  auto adjacencies = registry.view<Adjacencies>();
  adjacencies.each([&](Adjacencies& adjacencies) 
  {
    adjacencies.reset();
  });
}

void updateFakePlatforms(entt::registry& registry)
{
  auto fakePlatforms = registry.view<Fake, Collider, Animation>();
  fakePlatforms.each([&](Fake& fake, Collider& collider, Animation& animation) 
  {
    const Uint64 currentTime = SDL_GetTicks();
    const Uint64 timeElapsedSinceTriggered = currentTime - fake.collisionTime;
    if(fake.state == Fake::State::TRIGGERED && timeElapsedSinceTriggered >= fake.timeUntilFlipMS)
    {
      fake.state = Fake::State::FLIPPING;
      collider.enabled = false;
      animation.currentFrame = 0;
      animation.isPlaying = true;
    }
    else if(timeElapsedSinceTriggered >= fake.timeUntilFlipMS + fake.flipDuration)
    {
      fake.state = Fake::State::IDLE;
      collider.enabled = true;
    }
  });
}