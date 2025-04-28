#include "SDLApp.h"
#include "Constants.h"
#include "Screen/Screen.h"

#include <SDL3/SDL.h>
#include <imgui.h>
#include <imgui_impl_sdl3.h>
#include <imgui_impl_sdlrenderer3.h>

#include <string>

void SDLApp::initSDL(const std::string name, const int width, const int height)
{
    if (!SDL_Init(SDL_INIT_VIDEO))
    {
      SDL_LogError(SDL_LOG_CATEGORY_ERROR, "SDL_Init failed: %s", SDL_GetError());
      SDL_Quit();
      exit(1);
    }
  
    window = SDL_CreateWindow(name.c_str(), width, height, 0);
    if (!window)
    {
      SDL_LogError(SDL_LOG_CATEGORY_ERROR, "SDL_CreateWindow failed: %s", SDL_GetError());
      SDL_Quit();
      exit(1);
    }
  
    renderer = SDL_CreateRenderer(window, nullptr);
    if (!renderer)
    {
      SDL_LogError(SDL_LOG_CATEGORY_ERROR, "SDL_CreateRenderer failed: %s", SDL_GetError());
      SDL_DestroyWindow(window);
      SDL_Quit();
      exit(1);
    }
}

void SDLApp::initImGUI()
{
    ImGui::CreateContext();
    ImGui_ImplSDL3_InitForSDLRenderer(window, renderer);
    ImGui_ImplSDLRenderer3_Init(renderer);
}

void SDLApp::init(const std::string name, const int width, const int height)
{
  initSDL(name, width, height);
  initImGUI();
}

SDLApp::~SDLApp()
{
    delete currentScreen;
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
}

void SDLApp::changeScreen(Screen* screen)
{
    if(currentScreen)
    {
        delete currentScreen;
    }
    currentScreen = screen;
}

void SDLApp::run()
{
    bool quit = false;
    SDL_Event event;

    while (!quit)
    {
      Uint64 frameStartTime = SDL_GetTicks();
      while (SDL_PollEvent(&event))
      {
        if (event.type == SDL_EVENT_QUIT)
        {
          quit = true;
        }
        currentScreen->handleEvents(event);
      }

      update();
  
      render();
  
      Uint64 frameEndTime = SDL_GetTicks();
      Uint64 elapsedTime = frameEndTime - frameStartTime;
      if(elapsedTime < 1000/FPS)
      {
        SDL_Delay(1000/FPS - elapsedTime);
      }
    }
}

void SDLApp::update()
{
    currentScreen->update(0);
}

void SDLApp::render()
{
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);

    ImGui_ImplSDLRenderer3_NewFrame();
    ImGui_ImplSDL3_NewFrame();
    ImGui::NewFrame();

    currentScreen->render(renderer);

    ImGui::Render();
    ImGui_ImplSDLRenderer3_RenderDrawData(ImGui::GetDrawData(), renderer);

    SDL_RenderPresent(renderer);
}