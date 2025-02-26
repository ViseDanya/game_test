#pragma once

#include "Networking/ENetServer.h"
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

    void handleClientConnected(ENetEvent event);
    void handleMessageReceived(ENetEvent event);
    void handleClientDisconnected(ENetEvent event);
};