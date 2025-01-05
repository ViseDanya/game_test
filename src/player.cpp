#include "player.h"
#include "constants.h"
#include "color_renderable.h"

player::player(vec2 position) : dynamic_object(aabb(position, vec2(PLAYER_WIDTH/2, PLAYER_HEIGHT/2)), new color_renderable(PLAYER_COLOR)) {};