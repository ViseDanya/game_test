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
    void createGameScene();
    glm::vec2 getCeilingPosition();
    void updateCeiling();
    entt::registry registry;
    Camera camera;
    entt::entity ceiling;
    float platformSpawnPoint;
    float wallSpawnPoint;
};