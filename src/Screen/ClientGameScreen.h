#pragma once

#include "Screen.h"
#include "GameClient.h"

class ClientGameScreen : public Screen
{
public:
    ClientGameScreen(GameClient* client);
    ~ClientGameScreen();
    virtual void update(const float dt) override;
    virtual void render(SDL_Renderer* renderer) override;

private:
    void sendInput();

    GameClient* client;
};