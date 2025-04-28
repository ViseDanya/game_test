#include "MainMenuScreen.h"
#include "Screen/LocalGameScreen.h"
#include "SDLApp.h"

#include <imgui.h>
#include <imgui_impl_sdl3.h>
#include <imgui_impl_sdlrenderer3.h>

#include <iostream>

void MainMenuScreen::render(SDL_Renderer* renderer)
{
    ImGui::SetNextWindowPos({10, 10});
    ImGui::Begin("Main Menu", nullptr, ImGuiWindowFlags_AlwaysAutoResize);

    if(ImGui::Button("Local 1 Player"))
    {
        SDLApp& app = SDLApp::getInstance();
        app.changeScreen(new LocalGameScreen());
    }

    if(ImGui::Button("Local 2 Players"))
    {
    }

    if(ImGui::Button("Online Multiplayer"))
    {
    }

    if(ImGui::Button("Host Server"))
    {
    }

    ImGui::End();
}