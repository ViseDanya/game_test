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
    entt::entity playerEntity;
    std::unordered_map<entt::entity, entt::entity> serverToClientEntityMap;

    void handleMessageReceived(ENetEvent event);
    void handleServerDisconnected(ENetEvent event);

    void processAndSendInput(const bool* keystate);
};