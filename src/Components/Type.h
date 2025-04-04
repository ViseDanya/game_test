#pragma once

enum class EntityType
{
    PLAYER, NORMAL_PLATFORM, CONVEYOR_RIGHT, CONVEYOR_LEFT, TRAMPOLINE, WALL, SPIKES, FAKE_PLATFORM, CEILING
};

struct TypeComponent
{
    EntityType type;

    bool isPlatform() const
    {
        return type == EntityType::NORMAL_PLATFORM ||
               type == EntityType::CONVEYOR_RIGHT ||
               type == EntityType::CONVEYOR_LEFT ||
            //    type == EntityType::TRAMPOLINE ||
               type == EntityType::SPIKES ||
               type == EntityType::FAKE_PLATFORM;
    }
};