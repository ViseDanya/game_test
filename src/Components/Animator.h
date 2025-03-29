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
        FALL_RIGHT,
        IDLE_DAMAGED,
        RUN_LEFT_DAMAGED,
        RUN_RIGHT_DAMAGED,
        FALL_IDLE_DAMAGED,
        FALL_LEFT_DAMAGED,
        FALL_RIGHT_DAMAGED,
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
        animator.animations[IDLE_DAMAGED] = Animation::createPlayerIdleDamagedAnimation();
        animator.animations[RUN_LEFT_DAMAGED] = Animation::createPlayerRunLeftDamagedAnimation();
        animator.animations[RUN_RIGHT_DAMAGED] = Animation::createPlayerRunRightDamagedAnimation();
        animator.animations[FALL_IDLE_DAMAGED] = Animation::createPlayerFallIdleDamagedAnimation();
        animator.animations[FALL_RIGHT_DAMAGED] = Animation::createPlayerFallRightDamagedAnimation();
        animator.animations[FALL_LEFT_DAMAGED] = Animation::createPlayerFallLeftDamagedAnimation();
        return animator;
    }
};