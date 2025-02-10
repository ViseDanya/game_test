#pragma once

#include <cstdint>
#include <string.h>

enum MessageType
{
    PLAYER_DATA
};

struct PlayerData
{
    float x;
    float y;
};

void CreatePlayerMessage(const PlayerData& playerData, uint8_t* data)
{
    MessageType type = PLAYER_DATA;
    memcpy(data, (uint8_t*)&type, sizeof(MessageType));
    memcpy(data+sizeof(MessageType), (uint8_t*)&playerData, sizeof(playerData));
}