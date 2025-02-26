#pragma once

#include <enet/enet.h>
#include <functional>

class ENetServer
{
public:
    ENetServer() = default;
    ~ENetServer();
    
    void startServer();
    void broadcastMessageToClients(const char* data, const int length);
    void processEvents();

    std::function<void(ENetEvent)> handleClientConnected;
    std::function<void(ENetEvent)> handleMessageReceived;
    std::function<void(ENetEvent)> handleClientDisconnected;

private:
    ENetHost* server;
};