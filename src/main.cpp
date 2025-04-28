#include "GameServer.h"
#include "GameClient.h"
#include "LocalGame.h"
#include "Constants.h"

#include "SDLApp.h"
#include "Screen/MainMenuScreen.h"

#include <imgui.h>
#include <imgui_impl_sdl3.h>
#include <imgui_impl_sdlrenderer3.h>
#include <entt/entt.hpp>
#include <iostream>

SDL_Window* window = nullptr;
SDL_Renderer* sdlRenderer = nullptr;

void initializeEnet()
{
  if (enet_initialize() != 0)
  {
      std::cerr << "An error occurred while initializing ENet." << std::endl;
  }
  atexit (enet_deinitialize);
}

void initializeSDL()
{
  if (!SDL_Init(SDL_INIT_VIDEO))
  {
    SDL_LogError(SDL_LOG_CATEGORY_ERROR, "SDL_Init failed: %s", SDL_GetError());
    SDL_Quit();
    exit(1);
  }

  window = SDL_CreateWindow("Game", WINDOW_WIDTH, WINDOW_HEIGHT, 0);
  if (window == NULL)
  {
    SDL_LogError(SDL_LOG_CATEGORY_ERROR, "SDL_CreateWindow failed: %s", SDL_GetError());
    SDL_Quit();
    exit(1);
  }

  sdlRenderer = SDL_CreateRenderer(window, nullptr);
  if (sdlRenderer == nullptr){
    SDL_LogError(SDL_LOG_CATEGORY_ERROR, "SDL_CreateRenderer failed: %s", SDL_GetError());
    SDL_DestroyWindow(window);
    SDL_Quit();
    exit(1);
  }
}

void initializeImGui()
{
  ImGui::CreateContext();

  ImGui_ImplSDL3_InitForSDLRenderer(window, sdlRenderer);
  ImGui_ImplSDLRenderer3_Init(sdlRenderer);
}

void cleanup()
{
  ImGui_ImplSDL3_Shutdown();
  ImGui_ImplSDLRenderer3_Shutdown();
  ImGui::DestroyContext();
  SDL_DestroyWindow(window);
  SDL_Quit();
}

int main(int argc, char *argv[])
{
  // #ifndef HEADLESS
  // initializeSDL();
  // initializeImGui();
  // #endif

  initializeEnet();
  
  // if(argc == 2)
  // {
  //   std::string arg = argv[1];
  //   if(arg == "server")
  //   {
  //     GameServer gameServer;
  //     gameServer.run();
  //   }
  //   else if(arg == "client")
  //   {
  //     GameClient gameClient;
  //     gameClient.run();
  //   }
  // }
  // else
  // {
  //   LocalGame localGame;
  //   localGame.run();
  // }

  SDLApp& app = SDLApp::getInstance();
  app.init("Game", WINDOW_WIDTH, WINDOW_HEIGHT);
  app.changeScreen(new MainMenuScreen());
  app.run();
  
  cleanup();
}
