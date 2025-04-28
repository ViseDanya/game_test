#pragma once

#include "Screen.h"

#include <string>

class ClientConnectToServerScreen : public Screen
{
public:
    virtual void render(SDL_Renderer* renderer) override;

private:
    char serverAddress[256] = "localhost";
    char clientName[256] = "Sexy Beast";
};