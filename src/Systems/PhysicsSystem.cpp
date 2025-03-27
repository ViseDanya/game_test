#include "PhysicsSystem.h"
#include "Components/Box.h"
#include "Components/Velocity.h"
#include "Components/Adjacencies.h"
#include "Components/Fake.h"
#include "Components/Trampoline.h"
#include "Components/Collider.h"
#include "Components/Animation.h"
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

void updateTrampolines(entt::registry& registry)
{
  auto trampolines = registry.view<Trampoline>();
  trampolines.each([&](Trampoline& trampoline) 
  {
    if(trampoline.state == Trampoline::State::TRIGGERED)
    {
      trampoline.state = Trampoline::State::IDLE;
    }
  });
}

void updateFakePlatforms(entt::registry& registry)
{
  auto fakePlatforms = registry.view<Fake, Collider>();
  fakePlatforms.each([&](Fake& fake, Collider& collider) 
  {
    const Uint64 currentTime = SDL_GetTicks();
    const Uint64 timeElapsedSinceTriggered = currentTime - fake.collisionTime;
    if(fake.state == Fake::State::TRIGGERED && timeElapsedSinceTriggered >= fake.timeUntilFlipMS)
    {
      fake.state = Fake::State::FLIP;
      collider.isEnabled = false;
    }
    else if(fake.state == Fake::State::FLIP)
    {
      fake.state = Fake::State::FLIPPING;
    }
    else if(timeElapsedSinceTriggered >= fake.timeUntilFlipMS + fake.flipDuration)
    {
      fake.state = Fake::State::IDLE;
      collider.isEnabled = true;
    }
  });
}