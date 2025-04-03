#pragma once

#include "Networking/ENetServer.h"
#include "proto/game.pb.h"
#include "entt/entt.hpp"
#include <unordered_map>
#include <imgui.h>
#include <imgui_impl_sdl3.h>
#include <imgui_impl_sdlrenderer3.h>

class GameServer : public ENetServer
{
    public:
    
    struct ClientInfo
    {
        entt::entity player;
        game::PlayerInputMessage playerInputState;
        bool ready;
    };

    GameServer() = default;
    ~GameServer() = default;

    void run();

    private:
    entt::registry registry;
    std::unordered_map<enet_uint16, ClientInfo> gameClients;

    void handlePlayerInput(entt::entity player, const game::PlayerInputMessage& playerInputMessage);
    void broadcastGameUpdates();
    void broadcastDynamicEntityUpdates();
    void broadcastCameraUpdates();
    void broadcastTrampolineUpdates();
    void broadcastFakeUpdates();
    void broadcastHealthUpdates();

    void handleClientConnected(const ENetEvent& event) override;
    void handleMessageReceived(const ENetEvent& event) override;
    void handleClientDisconnected(const ENetEvent& event) override;

    void sendMessageToClient(enet_uint16 clientID, const game::Message& message);
    void broadcastMessageToClients(const game::Message& message);

    void resetCamera();
    void showImGui();
    void spawnPlatform();
    void spawnWalls();
    void createGameScene();

    float platformSpawnPoint;
    float wallSpawnPoint;

    void onEntityCreated(entt::entity entity);

    bool areAllPlayersReady();
    void startGame();
};