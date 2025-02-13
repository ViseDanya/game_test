#pragma once

#include "StreamBuffer.h"

enum MessageType
{
    PLAYER_DATA
};

struct PlayerPositionMessage
{
    float x;
    float y;
};

std::vector<std::byte> GetPlayerMessageAsBytes(const PlayerPositionMessage& playerData)
{
    std::vector<std::byte> buffer;
    StreamBuffer stream(buffer);
    stream.write(playerData);
    return buffer;
}