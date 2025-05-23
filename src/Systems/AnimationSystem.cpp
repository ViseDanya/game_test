#include "AnimationSystem.h"
#include "Components/Adjacencies.h"
#include "Components/Animation.h"
#include "Components/Animator.h"
#include "Components/Sprite.h"
#include "Components/Trampoline.h"
#include "Components/Fake.h"
#include "Components/Health.h"
#include <Velocity.h>

void updateAnimations(entt::registry& registry)
{
  auto view = registry.view<Sprite, Animation>();
  view.each([&](Sprite& sprite, Animation& animation) 
  {
    if(animation.isPlaying)
    {
      const Uint64 currentTime = SDL_GetTicks();
      const Uint64 elapsedTime = currentTime - animation.frameStartTimeMS;
      if(elapsedTime > 1000./animation.frameRate)
      {
        animation.currentFrame = (animation.currentFrame + 1) % animation.frames.size();
        animation.frameStartTimeMS = currentTime;
        if(!animation.isLooping && animation.currentFrame == 0)
        {
          animation.isPlaying = false;
        }
      }

      sprite.sourceRect = animation.frames.at(animation.currentFrame);
    }
  });
}

void updateAnimators(entt::registry& registry)
{
  auto view = registry.view<const Velocity, const Adjacencies, const Health, Animation, Animator>();
  view.each([&](const Velocity& velocity, const Adjacencies& adjacencies, const Health& health, Animation& animation, Animator& animator) 
  {
    Animator::State nextState;
    if(velocity.velocity.x > 0)
    {
      if(!adjacencies.isOnFloor)
      {
        if(health.state == Health::State::DAMAGED)
        {
          nextState = Animator::State::FALL_RIGHT_DAMAGED;
        }
        else
        {
          nextState = Animator::State::FALL_RIGHT;
        }
      }
      else
      {
        if(health.state == Health::State::DAMAGED)
        {
          nextState = Animator::State::RUN_RIGHT_DAMAGED;
        }
        else
        {
          nextState = Animator::State::RUN_RIGHT;
        }
      }
    }
    else if(velocity.velocity.x < 0)
    {
      if(!adjacencies.isOnFloor)
      {
        if(health.state == Health::State::DAMAGED)
        {
          nextState = Animator::State::FALL_LEFT_DAMAGED;
        }
        else
        {
          nextState = Animator::State::FALL_LEFT;
        }
      }
      else
      {
        if(health.state == Health::State::DAMAGED)
        {
          nextState = Animator::State::RUN_LEFT_DAMAGED;
        }
        else
        {
          nextState = Animator::State::RUN_LEFT;
        }
      }
    }
    else
    {
      if(!adjacencies.isOnFloor)
      {
        if(health.state == Health::State::DAMAGED)
        {
          nextState = Animator::State::FALL_IDLE_DAMAGED;
        }
        else
        {
          nextState = Animator::State::FALL_IDLE;
        }
      }
      else
      {
        if(health.state == Health::State::DAMAGED)
        {
          nextState = Animator::State::IDLE_DAMAGED;
        }
        else
        {
          nextState = Animator::State::IDLE;
        }
      }
  }

  if(nextState != animator.currentState)
  {
    animator.currentState = nextState;
    animation = animator.animations[nextState];
  }
  });
}

void updateTrampolineAnimations(entt::registry& registry)
{
  auto view = registry.view<Trampoline, Animation>();
  view.each([&](Trampoline& trampoline, Animation& animation) 
  {
    if(trampoline.state == Trampoline::State::TRIGGERED)
    {
      animation.currentFrame = 0;
      animation.isPlaying = true;
    }
  });
}

void updateFakeAnimations(entt::registry& registry)
{
  auto view = registry.view<Fake, Animation>();
  view.each([&](Fake& fake, Animation& animation) 
  {
    if(fake.state == Fake::State::FLIP)
    {
      animation.currentFrame = 0;
      animation.isPlaying = true;
    }
  });
}