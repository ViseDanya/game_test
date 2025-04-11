#pragma once

#include <enet/enet.h>

class ENetClient
{
public:
    ENetClient();
    ~ENetClient();
    
    void connectToServer(const char* address);
    void sendMessageToServer(const char* data, const int length);
    void processEvents();
private:
    ENetHost* client;
    ENetPeer* server;

    virtual void handleMessageReceived(const ENetEvent& event) = 0;
    virtual void handleServerDisconnected(const ENetEvent& event) = 0;
    virtual void handleServerConnected(const ENetEvent& event) = 0;
};