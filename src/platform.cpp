#include "platform.h"
#include "constants.h"
#include "color_renderable.h"

platform::platform(vec2 position) : game_object(aabb(position, vec2(PLATFORM_WIDTH/2, PLATFORM_HEIGHT/2)), new color_renderable(PLATFORM_COLOR)) {}