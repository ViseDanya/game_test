#include "game.pb.h"
#include "Entity.h"
#include "Camera.h"

game::Message createCreateEntityMessage(const entt::entity entity, const EntityType entityType, const glm::vec2& position);
game::Message createDynamicEntityUpdateMessage(entt::registry& registry, const entt::entity entity);
game::Message createCameraUpdateMessage(const Camera& camera);
game::Message createPlayAnimationMessage(const entt::entity entity);