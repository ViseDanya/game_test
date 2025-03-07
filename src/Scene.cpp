#include "Scene.h"
#include <random>
#include "Entity.h"
#include "Constants.h"

entt::entity generatePlatform(entt::registry& registry, glm::vec2 position)
{
  std::random_device rd;
  std::mt19937 gen(rd());
  std::uniform_int_distribution<> distr(0, NumPlatformTypes-1);
  switch(distr(gen))
  {
    case(PlatformType::NORMAL):
      return createNormalEntity(registry, position);
    case(PlatformType::CONVEYOR_LEFT):
      return createConveyorLeftEntity(registry, position);
    case(PlatformType::CONVEYOR_RIGHT):
      return createConveyorRightEntity(registry, position);
    case(PlatformType::TRAMPOLINE):
      return createTrampolineEntity(registry, position);
    case(PlatformType::SPIKES):
      return createSpikesEntity(registry, position);
    case(PlatformType::FAKE):
      return createFakeEntity(registry, position);
    default:
      return createNormalEntity(registry, position);
  }
}

void createTestScene(entt::registry& registry)
{
  registry.clear();
  createPlayer1Entity(registry, glm::vec2(WINDOW_WIDTH/2, WINDOW_HEIGHT/2));
  createPlayer2Entity(registry, glm::vec2(WINDOW_WIDTH/2, WINDOW_HEIGHT/2));
  createPlayer3Entity(registry, glm::vec2(WINDOW_WIDTH/2, WINDOW_HEIGHT/2));

  createConveyorLeftEntity(registry, glm::vec2(50, 20));
  createTrampolineEntity(registry, glm::vec2(200, 20));
  createSpikesEntity(registry, glm::vec2(300, 20));
  createWallEntity(registry, glm::vec2(WALL_WIDTH/2, WINDOW_HEIGHT/2));
  createFakeEntity(registry, glm::vec2(350, 40));
}

void createEvenlySpacedPlatforms(entt::registry& registry, float startHeight, int numPlatformsToGenerate)
{
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> distr(WALL_WIDTH + PLATFORM_WIDTH/2, WINDOW_WIDTH-WALL_WIDTH-PLATFORM_WIDTH/2);
    for(int i = 0; i < numPlatformsToGenerate; i++)
    {
        const float platformHorizontalPosition = distr(gen);
        const float platformVerticalPosition = startHeight - i*3*PLATFORM_HEIGHT;
        generatePlatform(registry, glm::vec2(platformHorizontalPosition, platformVerticalPosition));
    }
}