#pragma once

#include "Networking/ENetServer.h"
#include "entt/entt.hpp"

class GameServer : public ENetServer
{
    public:
    GameServer() = default;
    ~GameServer() = default;

    void run();

    private:
    entt::registry registry;

    void handleClientConnected(const ENetEvent& event) override;
    void handleMessageReceived(const ENetEvent& event) override;
    void handleClientDisconnected(const ENetEvent& event) override;
};