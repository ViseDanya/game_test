#include "SDL3/SDL.h"

#include "imgui.h"
#include "imgui_impl_sdl3.h"
#include "imgui_impl_sdlrenderer3.h"

#include "Constants.h"
#include "Client.hpp"
#include "Entity.h"
#include "TextureManager.h"
#include "Systems/RenderSystem.h"
#include "Systems/AnimationSystem.h"
#include "Systems/CollisionSystem.h"
#include "Systems/InputSystem.h"
#include "Systems/PhysicsSystem.h"
#include <iostream>
#include <entt/entt.hpp>
#include <numeric>

static SDL_Window* window = nullptr;
static SDL_Renderer* sdlRenderer = nullptr;

static bool gravityEnabled = false;

void Init_Enet()
{
  if (enet_initialize() != 0)
  {
      std::cerr << "An error occurred while initializing ENet." << std::endl;
  }
  atexit (enet_deinitialize);
}

void Init_SDL()
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

void Init_ImGui()
{
  IMGUI_CHECKVERSION();
  ImGui::CreateContext();
  ImGuiIO& io = ImGui::GetIO();
  io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;

  ImGui_ImplSDL3_InitForSDLRenderer(window, sdlRenderer);
  ImGui_ImplSDLRenderer3_Init(sdlRenderer);
}

void Show_ImGui()
{
    ImGui_ImplSDLRenderer3_NewFrame();
    ImGui_ImplSDL3_NewFrame();
    ImGui::NewFrame();
    ImGui::SetNextWindowSize({0, 0});
    ImGui::SetNextWindowPos({10, 10});
    ImGui::Begin("Options");
    ImGui::Checkbox("Gravity", &gravityEnabled);
    ImGui::End();

    ImGui::Render();
    ImGui_ImplSDLRenderer3_RenderDrawData(ImGui::GetDrawData(), sdlRenderer);
}

void Cleanup()
{
  ImGui_ImplSDL3_Shutdown();
  ImGui_ImplSDLRenderer3_Shutdown();
  ImGui::DestroyContext();
  SDL_DestroyWindow(window);
  SDL_Quit();
}

int main(int argc, char *argv[])
{
  Init_SDL();
  Init_ImGui();
  Init_Enet();
  
  TextureManager::loadAllTextures(sdlRenderer);
  Renderer renderer(sdlRenderer);

  bool quit = false;
  SDL_Event event;

  Camera camera;
  camera.position = glm::vec2(WINDOW_WIDTH/2, WINDOW_HEIGHT/2);
  camera.zoom = 1;
  float mouseX;
  float mouseY;

  entt::registry registry;
  createPlayer1Entity(registry);
  createPlayer2Entity(registry);
  createPlayer3Entity(registry);

  createConveyorEntity(registry);
  createTrampolineEntity(registry);
  createWallEntity(registry);

  while (!quit)
  {
    Uint64 frameStartTime = SDL_GetTicks();
    while (SDL_PollEvent(&event) != 0)
    {
      ImGui_ImplSDL3_ProcessEvent(&event);
      if (event.type == SDL_EVENT_QUIT)
      {
        quit = true;
      }
      else if (event.type == SDL_EVENT_MOUSE_WHEEL)
      {
        camera.zoom *= (event.wheel.y > 0) ? 1.1f : 0.9f; // Zoom in if scrolling up, zoom out if scrolling down
      }
    }

    float prevMouseX = mouseX;
    float prevMouseY = mouseY;
    if (SDL_GetMouseState(&mouseX, &mouseY) & SDL_BUTTON_LMASK)
    {
      float deltaX = mouseX - prevMouseX;
      float deltaY = mouseY - prevMouseY;
      camera.position.x -= (deltaX / camera.zoom);
      camera.position.y += (deltaY / camera.zoom);
    }

    std::cout << camera.zoom << std::endl;
    std::cout << camera.position.x << std::endl;

    const bool *keystate = SDL_GetKeyboardState(nullptr);

    SDL_SetRenderDrawColor(sdlRenderer, 0, 0, 0, 255);
    SDL_RenderClear(sdlRenderer);

    Show_ImGui();

    resetVelocity(registry, gravityEnabled);
    applyInputToVelocity(registry, keystate, gravityEnabled);
    resetAdjacencies(registry);
    applyVelocityToPosition(registry);
    resolveCollisions(registry);

    updateAnimators(registry);
    updateAnimations(registry);
    renderColoredEntities(registry, renderer, camera);
    renderSprites(registry, renderer, camera);

    SDL_RenderPresent(sdlRenderer);

    Uint64 frameEndTime = SDL_GetTicks();
    Uint64 elapsedTime = frameEndTime - frameStartTime;
    if(elapsedTime < 1000/FPS)
    {
      SDL_Delay(1000/FPS - elapsedTime);
    }
  }

  Cleanup();
}
