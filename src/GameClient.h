#pragma once

#include "Networking/ENetClient.h"
#include "Camera.h"
#include "entt/entt.hpp"

class GameClient : public ENetClient
{
public:
    GameClient(std::string name);
    entt::registry registry;
    bool ready;
    Camera camera;

    void sendReady();

private:
    std::string name;
    entt::entity playerEntity;
    std::unordered_map<entt::entity, entt::entity> serverToClientEntityMap;

    void handleMessageReceived(const ENetEvent& event) override;
    void handleServerDisconnected(const ENetEvent& event) override;
    void handleServerConnected(const ENetEvent& event) override;
};