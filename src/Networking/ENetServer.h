#pragma once

#include <enet/enet.h>

class ENetServer
{
public:
    ENetServer() = default;
    ~ENetServer();
    
    void startServer();
    void broadcastMessageToClients(const char* data, const int length);
    void processEvents();

private:
    ENetHost* server;
    virtual void handleClientConnected(const ENetEvent& event) = 0;
    virtual void handleMessageReceived(const ENetEvent& event) = 0;
    virtual void handleClientDisconnected(const ENetEvent& event) = 0;
};