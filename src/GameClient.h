#pragma once

#include "Networking/ENetClient.h"
#include "entt/entt.hpp"

class GameClient : public ENetClient
{
    public:
    GameClient() = default;
    ~GameClient() = default;

    void run();

    private:
    entt::registry registry;
    entt::entity playerEntity;
    std::unordered_map<entt::entity, entt::entity> serverToClientEntityMap;

    void handleMessageReceived(const ENetEvent& event) override;
    void handleServerDisconnected(const ENetEvent& event) override;

    void processAndSendInput(const bool* keystate);
};