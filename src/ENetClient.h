#pragma once

#include <enet/enet.h>
#include <functional>

class ENetClient
{
public:
    ENetClient();
    ~ENetClient();
    
    void connectToServer(const char* address);
    void sendMessageToServer(const char* data, const int length);
    void processEvents();

    std::function<void(ENetEvent)> handleMessageReceived;
    std::function<void(ENetEvent)> handleServerDisconnected;

private:
    ENetHost* client;
    ENetPeer* server;
};