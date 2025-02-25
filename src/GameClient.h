#pragma once

#include "Networking/ENetClient.h"
#include "entt/entt.hpp"

class GameClient
{
    public:
    GameClient();
    ~GameClient() = default;

    void run();

    private:
    ENetClient client;
    entt::registry registry;
    std::unordered_map<entt::entity, entt::entity> clientToServerEntityMap;

    void handleMessageReceived(ENetEvent event);
    void handleServerDisconnected(ENetEvent event);
};