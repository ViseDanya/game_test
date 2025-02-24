#pragma once
#include <entt/entt.hpp>

void applyInputToVelocity(entt::registry& registry, const bool* keystate, const bool gravity_enabled);