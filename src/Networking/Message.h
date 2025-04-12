#include <game.pb.h>
#include "Entity.h"
#include <glm/vec2.hpp>

struct Camera;
struct Health;
struct Name;

game::Message createCreateEntityMessage(const entt::entity entity, const EntityType entityType, const glm::vec2& position);
game::Message createDynamicEntityUpdateMessage(entt::registry& registry, const entt::entity entity);
game::Message createCameraUpdateMessage(const Camera& camera);
game::Message createPlayAnimationMessage(const entt::entity entity);
game::Message createHealthUpdateMessage(const entt::entity entity, const Health& health);
game::Message createPositionUpdateMessage(const entt::entity entity, const glm::vec2& position);
game::Message createDestroyEntityMessage(const entt::entity entity);
game::Message createNameMessage(const entt::entity entity, const Name& name);