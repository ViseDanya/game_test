#include "game.pb.h"
#include "Entity.h"
#include "Camera.h"

game::Message createCreateEntityMessage(const entt::entity entity, const EntityType entityType, const glm::vec2& position);
game::Message createDynamicEntityUpdateMessage(entt::registry& registry, entt::entity entity);
game::Message createCameraUpdateMessage(const Camera& camera);