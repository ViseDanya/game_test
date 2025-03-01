#pragma once

#include "entt/entt.hpp"
#include <glm/vec2.hpp>
#include "Camera.h"

class LocalGame
{
    public:
    LocalGame() = default;
    ~LocalGame() = default;

    void run();

    private:
    void resetCamera();
    void showImGui();
    void spawnPlatform();
    void spawnWalls();

    entt::registry registry;
    Camera camera;
    float platformSpawnPoint;
    float wallSpawnPoint;
};