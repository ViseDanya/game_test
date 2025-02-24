#pragma once

#include "ENetServer.h"
#include "entt/entt.hpp"

class GameServer
{
    public:
    GameServer();
    ~GameServer() = default;

    void run();

    private:
    ENetServer server;
    entt::registry registry;
    std::unordered_map<entt::entity, entt::entity> clientToServerEntityMap;

    void handleClientConnected(ENetEvent event);
    void handleMessageReceived(ENetEvent event);
    void handleClientDisconnected(ENetEvent event);
};