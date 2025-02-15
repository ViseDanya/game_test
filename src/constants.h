#include "SDL3/SDL.h"

extern SDL_Window* window;
extern SDL_Renderer* renderer;

const int WINDOW_WIDTH = 384;
const int WINDOW_HEIGHT = 352;

const int FPS = 60;
const SDL_Color PLAYER_ONE_COLOR = {255, 0, 0, 255};
const SDL_Color PLAYER_TWO_COLOR = {0, 255, 0, 255};
const SDL_Color PLAYER_THREE_COLOR = {0, 0, 255, 255};
const SDL_Color BOX_COLOR = {150, 75, 0, 255};
const SDL_Color PLATFORM_COLOR = {0, 0, 255, 255};

const float COLLISION_TOLERANCE = .001f;