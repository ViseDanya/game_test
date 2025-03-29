#pragma once
#include <entt/entt.hpp>

void resetVelocity(entt::registry& registry, const bool gravityEnabled);
void applyVelocityToPosition(entt::registry& registry);
void resetVerticalVelocityForEntitiesOnFloor(entt::registry& registry);
void resetAdjacencies(entt::registry& registry);
