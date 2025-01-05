#include "SDL3/SDL.h"

extern SDL_Window* window;
extern SDL_Renderer* renderer;

const int WINDOW_WIDTH = 384;
const int WINDOW_HEIGHT = 352;
const int PLAYER_WIDTH = 32;
const int PLAYER_HEIGHT = 32;
const int PLATFORM_WIDTH = 96;
const int PLATFORM_HEIGHT = 16;

const int FPS = 60;
const SDL_Color PLAYER_COLOR = {255, 0, 0, 255};
const SDL_Color PLATFORM_COLOR = {0, 0, 255, 255};

const float COLLISION_TOLERANCE = .001f;

const float PLAYER_SPEED = 180;
const float PLAYER_JUMP_HEIGHT = 60;
const float PLAYER_JUMP_TIME_TO_APEX = .25;
const float GRAVITY = -2*PLAYER_JUMP_HEIGHT/(PLAYER_JUMP_TIME_TO_APEX*PLAYER_JUMP_TIME_TO_APEX);
const float PLAYER_JUMP_SPEED = (PLAYER_JUMP_HEIGHT - .5 * GRAVITY * PLAYER_JUMP_TIME_TO_APEX * PLAYER_JUMP_TIME_TO_APEX)/(PLAYER_JUMP_TIME_TO_APEX);