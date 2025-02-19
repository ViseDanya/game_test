#pragma once

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
#include "TextureManager.h"
#include <entt/entt.hpp>

entt::entity createPlayerEntity(entt::registry& registry, const TextureManager& textureManager)
{
    const entt::entity playerEntity = registry.create();
    registry.emplace<Box>(playerEntity, Box(glm::vec2(WINDOW_WIDTH/2, WINDOW_HEIGHT/2), glm::vec2(16, 16)));
    registry.emplace<Sprite>(playerEntity, textureManager.playerTexture);
    registry.emplace<Velocity>(playerEntity);
    registry.emplace<Mass>(playerEntity, 1.);
    registry.emplace<Adjacencies>(playerEntity);
    registry.emplace<Animation>(playerEntity, Animation::createPlayerIdleAnimation());
    registry.emplace<Animator>(playerEntity, Animator::createPlayerAnimtor());
    return playerEntity;
}

entt::entity createPlayer1Entity(entt::registry& registry, const TextureManager& textureManager)
{
    const entt::entity p1Entity = createPlayerEntity(registry, textureManager);
    InputController& p1InputController = registry.emplace<InputController>(p1Entity);
    p1InputController.left_key = SDL_SCANCODE_A;
    p1InputController.right_key = SDL_SCANCODE_D;
    p1InputController.up_key = SDL_SCANCODE_W;
    p1InputController.down_key = SDL_SCANCODE_S;
    return p1Entity;
}

entt::entity createPlayer2Entity(entt::registry& registry, const TextureManager& textureManager)
{
  const entt::entity p2Entity = createPlayerEntity(registry, textureManager);
  InputController& p2InputController = registry.emplace<InputController>(p2Entity);
  p2InputController.left_key = SDL_SCANCODE_LEFT;
  p2InputController.right_key = SDL_SCANCODE_RIGHT;
  p2InputController.up_key = SDL_SCANCODE_UP;
  p2InputController.down_key = SDL_SCANCODE_DOWN;
  return p2Entity;
}

entt::entity createPlayer3Entity(entt::registry& registry, const TextureManager& textureManager)
{
  const entt::entity p3Entity = createPlayerEntity(registry, textureManager);
  InputController& p3InputController = registry.emplace<InputController>(p3Entity);
  p3InputController.left_key = SDL_SCANCODE_J;
  p3InputController.right_key = SDL_SCANCODE_L;
  p3InputController.up_key = SDL_SCANCODE_I;
  p3InputController.down_key = SDL_SCANCODE_K;
  return p3Entity;
}

entt::entity createConveyorEntity(entt::registry& registry, const TextureManager& textureManager)
{
    const entt::entity platformEntity = registry.create();
    registry.emplace<Box>(platformEntity, Box(glm::vec2(WINDOW_WIDTH/2, WINDOW_HEIGHT/2 - PLAYER_HEIGHT * 2), glm::vec2(100, 8)));
    registry.emplace<Sprite>(platformEntity, textureManager.conveyorRightTexture, SDL_FRect{0,0,96,16});
    registry.emplace<Conveyor>(platformEntity, PLAYER_SPEED/2.);
    registry.emplace<Animation>(platformEntity, Animation::createConveyorAnimation());
    return platformEntity;
}

entt::entity createTrampolineEntity(entt::registry& registry, const TextureManager& textureManager)
{
    const entt::entity floor = registry.create();
    registry.emplace<Box>(floor, Box(glm::vec2(WINDOW_WIDTH/2, 0), glm::vec2(WINDOW_WIDTH/2, 8)));
    registry.emplace<Sprite>(floor, textureManager.normalTexture);
    registry.emplace<Trampoline>(floor);
    return floor;
}
