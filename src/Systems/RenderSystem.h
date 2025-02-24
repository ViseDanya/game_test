#pragma once

#include "Renderer.h"
#include <entt/entt.hpp>
#include <glm/vec2.hpp>

struct Camera
{
  glm::vec2 position;
  float zoom;
};

void renderColoredEntities(entt::registry& registry, Renderer& renderer, const Camera& camera);
void renderSprites(entt::registry& registry, Renderer& renderer, const Camera& camera);
void renderDebugColliders(entt::registry& registry, Renderer& renderer, const Camera& camera);