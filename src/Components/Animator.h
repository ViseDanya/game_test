#pragma once

#include "Animation.h"

struct Animator
{
    enum State
    {
        IDLE,
        RUN_LEFT,
        RUN_RIGHT,
        FALL_IDLE,
        FALL_LEFT,
        FALL_RIGHT
    };

    State currentState;
    std::unordered_map<State, Animation> animations;

    static Animator createPlayerAnimtor()
    {
        Animator animator;
        animator.currentState = IDLE;
        animator.animations[IDLE] = Animation::createPlayerIdleAnimation();
        animator.animations[RUN_LEFT] = Animation::createPlayerRunLeftAnimation();
        animator.animations[RUN_RIGHT] = Animation::createPlayerRunRightAnimation();
        animator.animations[FALL_IDLE] = Animation::createPlayerFallIdleAnimation();
        animator.animations[FALL_RIGHT] = Animation::createPlayerFallRightAnimation();
        animator.animations[FALL_LEFT] = Animation::createPlayerFallLeftAnimation();
        return animator;
    }
};