#pragma once

#include "Components/Type.h"
#include <glm/vec2.hpp>
#include <entt/entt.hpp>

entt::entity createEntity(const EntityType entityType, entt::registry& registry, const glm::vec2& position);
entt::entity createPlayerEntity(entt::registry& registry, const glm::vec2& position);
entt::entity createPlayer1Entity(entt::registry& registry, const glm::vec2& position);
entt::entity createPlayer2Entity(entt::registry& registry, const glm::vec2& position);
entt::entity createPlayer3Entity(entt::registry& registry, const glm::vec2& position);
entt::entity createNormalEntity(entt::registry& registry, const glm::vec2& position);
entt::entity createConveyorEntity(entt::registry& registry, const glm::vec2& position);
entt::entity createConveyorRightEntity(entt::registry& registry, const glm::vec2& position);
entt::entity createConveyorLeftEntity(entt::registry& registry, const glm::vec2& position);
entt::entity createTrampolineEntity(entt::registry& registry, const glm::vec2& position);
entt::entity createWallEntity(entt::registry& registry, const glm::vec2& position);
entt::entity createSpikesEntity(entt::registry& registry, const glm::vec2& position);
entt::entity createFakeEntity(entt::registry& registry, const glm::vec2& position);
