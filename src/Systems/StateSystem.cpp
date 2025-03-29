#include "StateSystem.h"
#include "Components/Fake.h"
#include "Components/Trampoline.h"
#include "Components/Collider.h"
#include "Components/Animation.h"
#include "Components/Health.h"

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

void updateHealth(entt::registry& registry)
{
  auto healths = registry.view<Health>();
  healths.each([&](Health& health) 
  {
    if(health.state == Health::State::DAMAGED)
    {
      const Uint64 currentTime = SDL_GetTicks();
      const Uint64 timeElapsedSinceDamaged = currentTime - health.damageTime;
      if(timeElapsedSinceDamaged >= health.damageDuration)
      {
        health.state = Health::State::IDLE;
      }
    }
  });
}