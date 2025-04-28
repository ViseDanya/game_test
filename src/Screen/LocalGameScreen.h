#pragma once

#include "Screen.h"

#include "entt/entt.hpp"
#include <glm/vec2.hpp>
#include "Camera.h"

class LocalGameScreen : public Screen
{
    public:
    LocalGameScreen() = default;
    ~LocalGameScreen() = default;

    void run();

    virtual void handleEvents(const SDL_Event& event) override;
    virtual void update(const float dt) override;
    virtual void render(SDL_Renderer* renderer) override;

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