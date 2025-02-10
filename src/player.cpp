#include "Player.h"
#include <iostream>

Player::Player(const vec2& position) : dynamic_object(aabb(position, vec2(PLAYER_WIDTH / 2, PLAYER_HEIGHT / 2))) {};