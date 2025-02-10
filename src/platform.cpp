#include "platform.h"
#include "constants.h"

static const int PLATFORM_WIDTH = 96;
static const int PLATFORM_HEIGHT = 16;

platform::platform(const vec2& position) : game_object(aabb(position, vec2(PLATFORM_WIDTH/2, PLATFORM_HEIGHT/2))) {}