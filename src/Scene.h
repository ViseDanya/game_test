#pragma once

#include <entt/entt.hpp>
#include <glm/vec2.hpp>

enum PlatformType
{
  NORMAL, CONVEYOR_LEFT, CONVEYOR_RIGHT, TRAMPOLINE, SPIKES, FAKE, NumPlatformTypes
};

entt::entity generatePlatform(entt::registry& registry, glm::vec2 position);
void createTestScene(entt::registry& registry);
void createEvenlySpacedPlatforms(entt::registry& registry, float startHeight, int numPlatformsToGenerate);