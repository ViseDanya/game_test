#pragma once

#include "entt/entt.hpp"

class LocalGame
{
    public:
    LocalGame() = default;
    ~LocalGame() = default;

    void run();

    private:
    entt::registry registry;
};