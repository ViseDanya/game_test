#pragma once

#include <unordered_map>
#include <enet/enet.h>

class ENetServer
{
public:
    ENetServer() = default;
    ~ENetServer();
    
    void startServer();
    void sendReliableMessage(enet_uint16 clientID, const char* data, const int length);
    void sendUnreliableMessage(enet_uint16 clientID, const char* data, const int length);
    void broadcastReliableMessage(const char* data, const int length);
    void broadcastUnreliableMessage(const char* data, const int length);
    void processEvents();

private:
    ENetHost* server;
    std::unordered_map<enet_uint16, ENetPeer*> clients;
    virtual void handleClientConnected(const ENetEvent& event) = 0;
    virtual void handleMessageReceived(const ENetEvent& event) = 0;
    virtual void handleClientDisconnected(const ENetEvent& event) = 0;
};