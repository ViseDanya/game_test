#include "Screen.h"

#include <imgui.h>
#include <imgui_impl_sdl3.h>
#include <imgui_impl_sdlrenderer3.h>

void Screen::handleEvents(const SDL_Event& event)
{
    ImGui_ImplSDL3_ProcessEvent(&event);
}