#pragma once

#include <entt/entt.hpp>

struct Camera;
struct SDL_Renderer;

void renderColoredEntities(entt::registry& registry, SDL_Renderer* renderer, const Camera& camera);
void renderSprites(entt::registry& registry, SDL_Renderer* renderer, const Camera& camera);
void renderDebugColliders(entt::registry& registry, SDL_Renderer* renderer, const Camera& camera);