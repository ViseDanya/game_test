#pragma once

enum class EntityType
{
    PLAYER, NORMAL_PLATFORM, CONVEYOR_RIGHT, CONVEYOR_LEFT, TRAMPOLINE, WALL, SPIKES, FAKE_PLATFORM
};

struct TypeComponent
{
    EntityType type;
};