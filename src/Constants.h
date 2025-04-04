#pragma once

#include <SDL3/SDL.h>
#include <glm/vec2.hpp>
#include <iostream>

const int WINDOW_WIDTH = 384;
const int WINDOW_HEIGHT = 352;

constexpr int PLAYER_WIDTH = 32;
constexpr int PLAYER_HEIGHT = 32;

constexpr int WALL_WIDTH = 16;
constexpr int WALL_HEIGHT = 352;

constexpr int PLATFORM_WIDTH = 96;
constexpr int PLATFORM_HEIGHT = 16;
constexpr int TRAMPOLINE_HEIGHT = 22;
constexpr int SPIKES_HEIGHT = 32;
constexpr int FAKE_HEIGHT = 36;

constexpr int CEILING_WIDTH = WINDOW_WIDTH-2*WALL_WIDTH;

const float PLAYER_SPEED = 180;
const float PLAYER_JUMP_HEIGHT = 60;
const float PLAYER_JUMP_TIME_TO_APEX = .25;
const float GRAVITY = -2*PLAYER_JUMP_HEIGHT/(PLAYER_JUMP_TIME_TO_APEX*PLAYER_JUMP_TIME_TO_APEX);
const float PLAYER_JUMP_SPEED = (PLAYER_JUMP_HEIGHT - .5 * GRAVITY * PLAYER_JUMP_TIME_TO_APEX * PLAYER_JUMP_TIME_TO_APEX)/(PLAYER_JUMP_TIME_TO_APEX);

const int FPS = 60;
const SDL_Color PLAYER_ONE_COLOR = {255, 0, 0, 255};
const SDL_Color PLAYER_TWO_COLOR = {0, 255, 0, 255};
const SDL_Color PLAYER_THREE_COLOR = {0, 0, 255, 255};
const SDL_Color BOX_COLOR = {150, 75, 0, 255};
const SDL_Color PLATFORM_COLOR = {0, 0, 255, 255};

namespace glm
{
    const glm::vec2 UP(0.,1.);
    const glm::vec2 DOWN(0.,-1.);
    const glm::vec2 LEFT(-1.,0.);
    const glm::vec2 RIGHT(1.,0.);
    const glm::vec2 ZERO(0.,0.);
}

std::ostream& operator<<(std::ostream& stream, const glm::vec2& v);


