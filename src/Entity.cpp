#include "Entity.h"
#include "Components/Box.h"
#include "Components/Sprite.h"
#include "Components/Velocity.h"
#include "Components/Adjacencies.h"
#include "Components/Mass.h"
#include "Components/InputController.h"
#include "Components/Animation.h"
#include "Components/Animator.h"
#include "Components/Trampoline.h"
#include "Components/Conveyor.h"
#include "Components/Collider.h"
#include "Components/Fake.h"
#include "Components/Health.h"
#include "Components/HealthChanger.h"
#include "TextureManager.h"
#include <entt/entt.hpp>

entt::entity createEntity(const EntityType entityType, entt::registry& registry, const glm::vec2& position)
{
  switch(entityType)
  {
    case EntityType::PLAYER:
      return createPlayerEntity(registry, position);
    case EntityType::NORMAL_PLATFORM:
      return createNormalEntity(registry, position);
    case EntityType::CONVEYOR_RIGHT:
      return createConveyorRightEntity(registry, position);
    case EntityType::CONVEYOR_LEFT:
      return createConveyorLeftEntity(registry, position);
    case EntityType::TRAMPOLINE:
      return createTrampolineEntity(registry, position);
    case EntityType::WALL:
      return createWallEntity(registry, position);
    case EntityType::SPIKES:
      return createSpikesEntity(registry, position);
    case EntityType::FAKE_PLATFORM:
      return createFakeEntity(registry, position);
  }
}

entt::entity createPlayerEntity(entt::registry& registry, const glm::vec2& position)
{
  std::cout << "createPlayerEntity" << std::endl;
    const entt::entity playerEntity = registry.create();
    const Box& box = registry.emplace<Box>(playerEntity, 
      Box(position, glm::vec2(PLAYER_WIDTH/2, PLAYER_WIDTH/2)));
    registry.emplace<Collider>(playerEntity, Box(glm::ZERO, box.size));
    registry.emplace<Sprite>(playerEntity, TextureManager::playerTexture);
    registry.emplace<Velocity>(playerEntity);
    registry.emplace<Mass>(playerEntity, 1.);
    registry.emplace<Adjacencies>(playerEntity);
    registry.emplace<Animation>(playerEntity, Animation::createPlayerIdleAnimation());
    registry.emplace<Animator>(playerEntity, Animator::createPlayerAnimtor());
    registry.emplace<Health>(playerEntity, 10);
    registry.emplace<TypeComponent>(playerEntity, EntityType::PLAYER);
    return playerEntity;
}

entt::entity createPlayer1Entity(entt::registry& registry, const glm::vec2& position)
{
    const entt::entity p1Entity = createPlayerEntity(registry, position);
    InputController& p1InputController = registry.emplace<InputController>(p1Entity);
    p1InputController.left_key = SDL_SCANCODE_A;
    p1InputController.right_key = SDL_SCANCODE_D;
    p1InputController.up_key = SDL_SCANCODE_W;
    p1InputController.down_key = SDL_SCANCODE_S;
    return p1Entity;
}

entt::entity createPlayer2Entity(entt::registry& registry, const glm::vec2& position)
{
  const entt::entity p2Entity = createPlayerEntity(registry, position);
  InputController& p2InputController = registry.emplace<InputController>(p2Entity);
  p2InputController.left_key = SDL_SCANCODE_LEFT;
  p2InputController.right_key = SDL_SCANCODE_RIGHT;
  p2InputController.up_key = SDL_SCANCODE_UP;
  p2InputController.down_key = SDL_SCANCODE_DOWN;
  return p2Entity;
}

entt::entity createPlayer3Entity(entt::registry& registry, const glm::vec2& position)
{
  const entt::entity p3Entity = createPlayerEntity(registry, position);
  InputController& p3InputController = registry.emplace<InputController>(p3Entity);
  p3InputController.left_key = SDL_SCANCODE_J;
  p3InputController.right_key = SDL_SCANCODE_L;
  p3InputController.up_key = SDL_SCANCODE_I;
  p3InputController.down_key = SDL_SCANCODE_K;
  return p3Entity;
}

entt::entity createNormalEntity(entt::registry& registry, const glm::vec2& position)
{
    const entt::entity normalEntity = registry.create();
    const Box& box = registry.emplace<Box>(normalEntity, 
      Box(position, glm::vec2(PLATFORM_WIDTH/2, PLATFORM_HEIGHT/2)));
    registry.emplace<Collider>(normalEntity, Box(glm::ZERO, box.size), true, true);
    registry.emplace<Sprite>(normalEntity, TextureManager::normalTexture, SDL_FRect{0,0,PLATFORM_WIDTH,PLATFORM_HEIGHT});
    registry.emplace<TypeComponent>(normalEntity, EntityType::NORMAL_PLATFORM);
    registry.emplace<HealthChanger>(normalEntity, 1);
    return normalEntity;
}

entt::entity createConveyorEntity(entt::registry& registry, const glm::vec2& position)
{
    const entt::entity conveyorEntity = registry.create();
    const Box& box = registry.emplace<Box>(conveyorEntity, 
      Box(position, glm::vec2(PLATFORM_WIDTH/2, PLATFORM_HEIGHT/2)));
    registry.emplace<Collider>(conveyorEntity, Box(glm::ZERO, box.size), true, true);
    registry.emplace<Conveyor>(conveyorEntity);
    registry.emplace<Animation>(conveyorEntity, Animation::createConveyorAnimation());
    registry.emplace<HealthChanger>(conveyorEntity, 1);
    return conveyorEntity;
}

entt::entity createConveyorRightEntity(entt::registry& registry, const glm::vec2& position)
{
    const entt::entity conveyorEntity = createConveyorEntity(registry, position);
    registry.emplace<Sprite>(conveyorEntity, TextureManager::conveyorRightTexture, SDL_FRect{0,0,PLATFORM_WIDTH,PLATFORM_HEIGHT});
    Conveyor& conveyor = registry.get<Conveyor>(conveyorEntity);
    conveyor.speed = PLAYER_SPEED/2;
    registry.emplace<TypeComponent>(conveyorEntity, EntityType::CONVEYOR_RIGHT);
    return conveyorEntity;
}

entt::entity createConveyorLeftEntity(entt::registry& registry, const glm::vec2& position)
{
    const entt::entity conveyorEntity = createConveyorEntity(registry, position);
    registry.emplace<Sprite>(conveyorEntity, TextureManager::conveyorLeftTexture, SDL_FRect{0,0,PLATFORM_WIDTH,PLATFORM_HEIGHT});
    Conveyor& conveyor = registry.get<Conveyor>(conveyorEntity);
    conveyor.speed = -PLAYER_SPEED/2;
    registry.emplace<TypeComponent>(conveyorEntity, EntityType::CONVEYOR_LEFT);
    return conveyorEntity;
}

entt::entity createTrampolineEntity(entt::registry& registry, const glm::vec2& position)
{
    const entt::entity trampoline = registry.create();
    const Box& box = registry.emplace<Box>(trampoline, position, 
      glm::vec2(PLATFORM_WIDTH/2, TRAMPOLINE_HEIGHT/2));
    registry.emplace<Collider>(trampoline, Box(glm::ZERO, box.size), true, true);
    registry.emplace<Sprite>(trampoline, TextureManager::trampolineTexture, SDL_FRect{0,0,PLATFORM_WIDTH,TRAMPOLINE_HEIGHT});
    registry.emplace<Trampoline>(trampoline);
    registry.emplace<Animation>(trampoline, Animation::createTrampolineAnimation());
    registry.emplace<TypeComponent>(trampoline, EntityType::TRAMPOLINE);
    registry.emplace<HealthChanger>(trampoline, 1);
    return trampoline;
}

entt::entity createWallEntity(entt::registry& registry, const glm::vec2& position)
{
    const entt::entity wall = registry.create();
    const Box& box = registry.emplace<Box>(wall, position, 
      glm::vec2(WALL_WIDTH/2, WINDOW_HEIGHT/2));
    registry.emplace<Collider>(wall, Box(glm::ZERO, box.size));
    registry.emplace<Sprite>(wall, TextureManager::wallTexture,SDL_FRect{0,0,WALL_WIDTH,WALL_HEIGHT});
    registry.emplace<TypeComponent>(wall, EntityType::WALL);
    return wall;
}

entt::entity createSpikesEntity(entt::registry& registry, const glm::vec2& position)
{
    const entt::entity spikes = registry.create();
    registry.emplace<Box>(spikes, Box(position, glm::vec2(PLATFORM_WIDTH/2, SPIKES_HEIGHT/2)));
    registry.emplace<Collider>(spikes, Box(glm::vec2(0,-SPIKES_HEIGHT/4), glm::vec2(PLATFORM_WIDTH/2,7)), true, true);
    registry.emplace<Sprite>(spikes, TextureManager::spikesTexture, SDL_FRect{0,0,PLATFORM_WIDTH,SPIKES_HEIGHT});
    registry.emplace<TypeComponent>(spikes, EntityType::SPIKES);
    registry.emplace<HealthChanger>(spikes, -1);
    return spikes;
}

entt::entity createFakeEntity(entt::registry& registry, const glm::vec2& position)
{
    const entt::entity fakeEntity = registry.create();
    const Box& box = registry.emplace<Box>(fakeEntity, 
      Box(position, glm::vec2(PLATFORM_WIDTH/2, FAKE_HEIGHT/2)));
    registry.emplace<Collider>(fakeEntity, Box(glm::ZERO, glm::vec2(PLATFORM_WIDTH/2, PLATFORM_HEIGHT/2)), true, true);
    registry.emplace<Sprite>(fakeEntity, TextureManager::fakeTexture, SDL_FRect{0,0,PLATFORM_WIDTH,FAKE_HEIGHT});
    registry.emplace<Animation>(fakeEntity, Animation::createFakeAnimation());
    registry.emplace<Fake>(fakeEntity);
    registry.emplace<TypeComponent>(fakeEntity, EntityType::FAKE_PLATFORM);
    registry.emplace<HealthChanger>(fakeEntity, 1);
    return fakeEntity;
}
