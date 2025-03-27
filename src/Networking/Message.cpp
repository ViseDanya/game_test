#include "Message.h"
#include "Components/Box.h"
#include "Components/Velocity.h"
#include "Components/Adjacencies.h"

game::Message createCreateEntityMessage(const entt::entity entity, const EntityType entityType, const glm::vec2& position)
{
    game::CreateEntityMessage createEntityMessage;
    createEntityMessage.set_entity(entt::to_integral(entity));
    createEntityMessage.set_entity_type(static_cast<game::EntityType>(entityType));
    game::vec2* pos = createEntityMessage.mutable_position();
    pos->set_x(position.x);
    pos->set_y(position.y);
    game::Message message;
    message.set_message_type(game::MessageType::CREATE_ENTITY_MESSAGE);
    message.mutable_create_entity_message()->CopyFrom(createEntityMessage);
    return message;
}

game::Message createDynamicEntityUpdateMessage(entt::registry& registry, entt::entity entity)
{
    game::DynamicEntityUpdateMessage dynamicEntityUpdateMessage;
    dynamicEntityUpdateMessage.set_entity(entt::to_integral(entity));

    const Box& box = registry.get<Box>(entity);
    game::vec2* position = dynamicEntityUpdateMessage.mutable_position();
    position->set_x(box.center.x);
    position->set_y(box.center.y);

    const Velocity& velocityComponent = registry.get<Velocity>(entity);
    game::vec2* velocity = dynamicEntityUpdateMessage.mutable_velocity();
    velocity->set_x(velocityComponent.velocity.x);
    velocity->set_y(velocityComponent.velocity.y);

    const Adjacencies& adjacencies= registry.get<Adjacencies>(entity);
    dynamicEntityUpdateMessage.set_is_on_floor(adjacencies.isOnFloor);

    game::Message message;
    message.set_message_type(game::MessageType::DYNAMIC_ENTITY_UPDATE_MESSAGE);
    message.mutable_dynamic_entity_update_messsage()->CopyFrom(dynamicEntityUpdateMessage);
    return message;
}

game::Message createCameraUpdateMessage(const Camera& camera)
{
    game::CameraUpdateMessage cameraUpdateMessage;
    game::vec2* cameraPosition = cameraUpdateMessage.mutable_position();
    cameraPosition->set_x(camera.position.x);
    cameraPosition->set_y(camera.position.y);
    cameraUpdateMessage.set_zoom(camera.zoom);
    game::Message message;
    message.set_message_type(game::MessageType::CAMERA_UPDATE_MESSAGE);
    message.mutable_camera_update_message()->CopyFrom(cameraUpdateMessage);
    return message;
}

game::Message createPlayAnimationMessage(const entt::entity entity)
{
    game::PlayAnimationMessage playAnimationMessage;
    playAnimationMessage.set_entity(entt::to_integral(entity));
    game::Message message;
    message.set_message_type(game::MessageType::PLAY_ANIMATION_MESSAGE);
    message.mutable_play_animation_message()->CopyFrom(playAnimationMessage);
    return message;
}