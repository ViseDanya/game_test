#pragma once
#include <glm/vec2.hpp>

enum Direction
{
  UP, DOWN, LEFT, RIGHT
};

Direction getOppositeDirection(const Direction d);
glm::vec2 getDirectionAsVector(const Direction d);