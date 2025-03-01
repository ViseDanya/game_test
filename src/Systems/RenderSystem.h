#pragma once

#include "Renderer.h"
#include "Camera.h"
#include <entt/entt.hpp>
#include <glm/vec2.hpp>

void renderColoredEntities(entt::registry& registry, Renderer& renderer, const Camera& camera);
void renderSprites(entt::registry& registry, Renderer& renderer, const Camera& camera);
void renderDebugColliders(entt::registry& registry, Renderer& renderer, const Camera& camera);