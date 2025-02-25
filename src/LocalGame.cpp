#include "LocalGame.h"
#include "Entity.h"
#include "Systems/RenderSystem.h"
#include "Systems/AnimationSystem.h"
#include "Systems/CollisionSystem.h"
#include "Systems/InputSystem.h"
#include "Systems/PhysicsSystem.h"
#include "Constants.h"
#include "TextureManager.h"
#include "Scene.h"
#include <imgui.h>
#include <imgui_impl_sdl3.h>
#include <imgui_impl_sdlrenderer3.h>

extern SDL_Renderer* sdlRenderer;
static Camera camera;

static bool gravityEnabled = false;
static bool manualCamera = true;
static bool debugColliders = false;

void resetCamera()
{
  camera.position = glm::vec2(WINDOW_WIDTH/2, WINDOW_HEIGHT/2);
  camera.zoom = 1;
}

void showImGui(entt::registry& registry)
{
    ImGui_ImplSDLRenderer3_NewFrame();
    ImGui_ImplSDL3_NewFrame();
    ImGui::NewFrame();
    ImGui::SetNextWindowSize({0, 0});
    ImGui::SetNextWindowPos({10, 10});
    ImGui::Begin("Options");
    ImGui::Checkbox("Gravity", &gravityEnabled);
    ImGui::Checkbox("Manual Camera", &manualCamera);
    ImGui::Checkbox("Debug Colliders", &debugColliders);
    if(ImGui::Button("Reset Test Scene"))
    {
      createTestScene(registry);
      resetCamera();
    }
    if(ImGui::Button("Reset Game Scene"))
    {
      createGameScene(registry);
      resetCamera();
    }
    ImGui::End();

    ImGui::Render();
    ImGui_ImplSDLRenderer3_RenderDrawData(ImGui::GetDrawData(), sdlRenderer);
}

void LocalGame::run()
{
    TextureManager::loadAllTextures(sdlRenderer);
    Renderer renderer(sdlRenderer);
  
    bool quit = false;
    SDL_Event event;
  
    float mouseX;
    float mouseY;
  
    entt::registry registry;
    createGameScene(registry);
    resetCamera();  
    
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
        else if (event.type == SDL_EVENT_MOUSE_WHEEL && manualCamera)
        {
          camera.zoom *= (event.wheel.y > 0) ? 1.1f : 0.9f; // Zoom in if scrolling up, zoom out if scrolling down
        }
      }
  
      if(manualCamera)
      {
        float prevMouseX = mouseX;
        float prevMouseY = mouseY;
        if (SDL_GetMouseState(&mouseX, &mouseY) & SDL_BUTTON_LMASK)
        {
          float deltaX = mouseX - prevMouseX;
          float deltaY = mouseY - prevMouseY;
          camera.position.x -= (deltaX / camera.zoom);
          camera.position.y += (deltaY / camera.zoom);
        }
      }
      else
      {
        camera.position.y -= (WINDOW_WIDTH/(3*FPS));
      }
  
      const bool *keystate = SDL_GetKeyboardState(nullptr);
        
      resetVelocity(registry, gravityEnabled);
      applyInputToVelocity(registry, keystate, gravityEnabled);
      resetAdjacencies(registry);
      applyVelocityToPosition(registry);
      resolveCollisions(registry);
      updateFakePlatforms(registry);
  
      SDL_SetRenderDrawColor(sdlRenderer, 0, 0, 0, 255);
      SDL_RenderClear(sdlRenderer);
  
      updateAnimators(registry);
      updateTrampolineAnimations(registry);
      updateAnimations(registry);
      renderColoredEntities(registry, renderer, camera);
      renderSprites(registry, renderer, camera);
  
      showImGui(registry);
  
      if(debugColliders)
      {
        renderDebugColliders(registry, renderer, camera);
      }
  
      SDL_RenderPresent(sdlRenderer);
  
      Uint64 frameEndTime = SDL_GetTicks();
      Uint64 elapsedTime = frameEndTime - frameStartTime;
      if(elapsedTime < 1000/FPS)
      {
        SDL_Delay(1000/FPS - elapsedTime);
      }
    }
}