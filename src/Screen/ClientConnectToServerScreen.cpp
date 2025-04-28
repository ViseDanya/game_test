#include "ClientConnectToServerScreen.h"
#include "Screen/ClientGameScreen.h"
#include "GameClient.h"
#include "SDLApp.h"

#include <imgui.h>

void ClientConnectToServerScreen::render(SDL_Renderer* renderer)
{
    ImGui::SetNextWindowPos({10, 10});
    ImGui::Begin("Options", nullptr, ImGuiWindowFlags_AlwaysAutoResize);

    ImGui::Text("Server Address");
    ImGui::SameLine();
    ImGui::InputText("##labelServer Address", serverAddress, IM_ARRAYSIZE(serverAddress));

    ImGui::Text("Name");
    ImGui::SameLine();
    ImGui::InputText("##labelName", clientName, IM_ARRAYSIZE(clientName));

    if (ImGui::Button("Connect"))
    {
        GameClient* client = new GameClient(clientName);
        if(client->connectToServer(serverAddress))
        {
            SDLApp::getInstance().changeScreen(new ClientGameScreen(client));
        }
        else
        {
            delete client;
        }
    }

    ImGui::End();
}