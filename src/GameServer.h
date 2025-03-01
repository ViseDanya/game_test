#pragma once

#include "Networking/ENetServer.h"
#include "game.pb.h"
#include "entt/entt.hpp"
#include <unordered_map>

class GameServer : public ENetServer
{
    public:
    GameServer() = default;
    ~GameServer() = default;

    void run();

    private:
    entt::registry registry;
    std::unordered_map<enet_uint16, entt::entity> players;
    std::unordered_map<enet_uint16, game::PlayerInputMessage> playerInputState;

    void handlePlayerInput(entt::entity player, const game::PlayerInputMessage& playerInputMessage);
    void broadcastUpdatesToClients();

    void handleClientConnected(const ENetEvent& event) override;
    void handleMessageReceived(const ENetEvent& event) override;
    void handleClientDisconnected(const ENetEvent& event) override;
};