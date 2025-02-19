#pragma once

#include <Adjacencies.h>
#include <Animation.h>
#include <Animator.h>
#include <Sprite.h>
#include <Velocity.h>
#include <entt/entt.hpp>

void updateAnimations(entt::registry& registry)
{
  auto view = registry.view<Sprite, Animation>();
  view.each([&](Sprite& sprite, Animation& animation) 
  {
    const Uint64 currentTime = SDL_GetTicks();
    const Uint64 elapsedTime = currentTime - animation.frameStartTimeMS;
    if(elapsedTime > 1000./animation.frameRate)
    {
      animation.currentFrame = (animation.currentFrame + 1) % animation.frames.size();
      animation.frameStartTimeMS = currentTime;
    }

    sprite.sourceRect = animation.frames.at(animation.currentFrame);
  });
}

void updateAnimators(entt::registry& registry)
{
  auto view = registry.view<Velocity, const Adjacencies, Animation, Animator>();
  view.each([&](Velocity& velocity, const Adjacencies& adjacencies, Animation& animation, Animator& animator) 
  {
    Animator::State nextState;
    if(velocity.velocity.x > 0)
    {
      if(!adjacencies.isOnFloor)
      {
        nextState = Animator::State::FALL_RIGHT;
      }
      else
      {
        nextState = Animator::State::RUN_RIGHT;
      }
    }
    else if(velocity.velocity.x < 0)
    {
      if(!adjacencies.isOnFloor)
      {
        nextState = Animator::State::FALL_LEFT;
      }
      else
      {
        nextState = Animator::State::RUN_LEFT;
      }
    }
    else
    {
      if(!adjacencies.isOnFloor)
      {
        nextState = Animator::State::FALL_IDLE;
      }
      else
      {
        nextState = Animator::State::IDLE;
      }
  }

  if(nextState != animator.currentState)
  {
    animator.currentState = nextState;
    animation = animator.animations[nextState];
  }
  });
}