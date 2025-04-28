#include "RenderSystem.h"

#include <Camera.h>
#include <Components/Collider.h>
#include <Constants.h>
#include <Box.h>
#include <Sprite.h>
#include <SDLRenderUtils.h>

#include <entt/entt.hpp>
#include <glm/vec2.hpp>

glm::vec2 worldToCamera(const glm::vec2& coord, const Camera& camera)
{
  return (coord - camera.position) * camera.zoom;
}

glm::vec2 cameraToScreen(const glm::vec2& coord)
{
  return glm::vec2(coord.x + WINDOW_WIDTH/2, WINDOW_HEIGHT/2 - coord.y);
}

SDL_FRect getDestinationRect(const Box& box, const Camera& camera)
{
  const glm::vec2 screenBoxPosition = cameraToScreen(worldToCamera(box.topLeft(), camera));
  const glm::vec2 screenBoxSize = box.size * 2.f * camera.zoom;
  SDL_FRect destRect = {screenBoxPosition.x, screenBoxPosition.y, screenBoxSize.x, screenBoxSize.y};
  return destRect;
}

void renderColoredEntities(entt::registry& registry, SDL_Renderer* renderer, const Camera& camera)
{
    auto view = registry.view<const Box, const SDL_Color>();
    view.each([&](const Box& box, const SDL_Color& color) 
        {
          renderColoredFilledRectangle(renderer, color, getDestinationRect(box, camera));
        });
};

void renderSprites(entt::registry& registry, SDL_Renderer* renderer, const Camera& camera)
{
    auto view = registry.view<const Box, const Sprite>();
    view.each([&](const Box& box, const Sprite& sprite) 
        {
          renderTexture(renderer, sprite.texture, sprite.sourceRect, getDestinationRect(box, camera));
        });
};

void renderDebugColliders(entt::registry& registry, SDL_Renderer* renderer, const Camera& camera)
{
    auto view = registry.view<const Box, const Collider>();
    view.each([&](const Box& box, const Collider& collider) 
        {
          const Box colliderInWorldSpace = Box(collider.box.center + box.center, collider.box.size);
          const SDL_Color color = collider.isEnabled ? SDL_Color{255,0,0,255} : SDL_Color{0,255,0,255};
          renderColoredRectangle(renderer, color, getDestinationRect(colliderInWorldSpace, camera));
        });
};