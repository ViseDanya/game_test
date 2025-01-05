#include "SDL3/SDL.h"

const int WINDOW_WIDTH = 384;
const int WINDOW_HEIGHT = 352;
const int PLAYER_WIDTH = 32;
const int PLAYER_HEIGHT = 32;

const int FPS = 60;
const int PLAYER_SPEED = 3;
const int GRAVITY = 2;

const int PLATFORM_WIDTH = 96;
const int PLATFORM_HEIGHT = 16;

extern SDL_Window* window;
extern SDL_Renderer* renderer;

const SDL_Color PLAYER_COLOR = {255, 0, 0, 255};
const SDL_Color PLATFORM_COLOR = {0, 0, 255, 255};

const float COLLISION_TOLERANCE = .001f;