#include "LocalGame.h"
#include "Entity.h"
#include "Systems/RenderSystem.h"
#include "Systems/AnimationSystem.h"
#include "Systems/CollisionSystem.h"
#include "Systems/InputSystem.h"
#include "Systems/PhysicsSystem.h"
#include "Systems/StateSystem.h"
#include "Constants.h"
#include "TextureManager.h"
#include "Components/Health.h"
#include "Scene.h"
#include <sstream>
#include <imgui.h>
#include <imgui_impl_sdl3.h>
#include <imgui_impl_sdlrenderer3.h>

extern SDL_Renderer* sdlRenderer;

static bool gravityEnabled = false;
static bool manualCamera = true;
static bool debugColliders = false;

void LocalGame::resetCamera()
{
  camera.position = glm::vec2(WINDOW_WIDTH/2, WINDOW_HEIGHT/2);
  camera.zoom = 1;
}

glm::vec2 LocalGame::getCeilingPosition()
{
  return glm::vec2(camera.position.x, camera.position.y + WINDOW_HEIGHT/2 - PLATFORM_HEIGHT/2);
}

void LocalGame::updateCeiling()
{
  Box& box = registry.get<Box>(ceiling);
  box.center.y -= CAMERA_SPEED;
}

void LocalGame::createGameScene()
{
  registry.clear();
  createPlayer1Entity(registry, glm::vec2(WINDOW_WIDTH/2, WINDOW_HEIGHT/2));
  createNormalEntity(registry, glm::vec2(WINDOW_WIDTH/2, WINDOW_HEIGHT/2 - PLAYER_HEIGHT/2 - PLATFORM_HEIGHT/2));
  resetCamera();
  platformSpawnPoint = WINDOW_HEIGHT/2 - PLAYER_HEIGHT/2 - PLATFORM_HEIGHT/2 - 3*PLATFORM_HEIGHT;
  for(int i = 0; i < 10; i++)
  {
    spawnPlatform();
  }

  wallSpawnPoint = WALL_HEIGHT/2;
  spawnWalls();

  ceiling = createCeilingEntity(registry, getCeilingPosition());
}

void LocalGame::showImGui()
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
      createGameScene();
    }
    auto view = registry.view<Health>();
    view.each([&](entt::entity e, const Health& h) 
    {
      std::stringstream ss;
      ss << "Player " << entt::to_integral(e) << " health: " << h.health;
      ImGui::Text("%s", ss.str().c_str());
    });

    ImGui::End();

    ImGui::Render();
    ImGui_ImplSDLRenderer3_RenderDrawData(ImGui::GetDrawData(), sdlRenderer);
}

void LocalGame::spawnPlatform()
{
  createEvenlySpacedPlatforms(registry, platformSpawnPoint, 1);
  platformSpawnPoint -= 3*PLATFORM_HEIGHT;
}

void LocalGame::spawnWalls()
{
  createWallEntity(registry, glm::vec2(WALL_WIDTH/2, wallSpawnPoint));
  createWallEntity(registry, glm::vec2(WINDOW_WIDTH - WALL_WIDTH/2, wallSpawnPoint));
  wallSpawnPoint -= WALL_HEIGHT;
}

void LocalGame::run()
{
    TextureManager::loadAllTextures(sdlRenderer);
    Renderer renderer(sdlRenderer);
  
    bool quit = false;
    SDL_Event event;
  
    float mouseX;
    float mouseY; 
    
    createGameScene();

    ceiling = createCeilingEntity(registry, getCeilingPosition());

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
        camera.position.y -= CAMERA_SPEED;
        updateCeiling();
      }
  
      const bool *keystate = SDL_GetKeyboardState(nullptr);

      if(camera.position.y - platformSpawnPoint < 2*WINDOW_HEIGHT)
      {
        spawnPlatform();
      }
      if(camera.position.y - wallSpawnPoint < 2*WINDOW_HEIGHT)
      {
        spawnWalls();
      }
        
      resetVelocity(registry, gravityEnabled);
      applyInputToVelocity(registry, keystate, gravityEnabled);
      resetAdjacencies(registry);
      applyVelocityToPosition(registry);
      updateTrampolines(registry);
      updateFakePlatforms(registry);
      updateHealth(registry);
      resolveCollisions(registry);
  
      SDL_SetRenderDrawColor(sdlRenderer, 0, 0, 0, 255);
      SDL_RenderClear(sdlRenderer);
  
      updateAnimators(registry);
      updateTrampolineAnimations(registry);
      updateFakeAnimations(registry);
      updateAnimations(registry);
      renderColoredEntities(registry, renderer, camera);
      renderSprites(registry, renderer, camera);
  
      showImGui();
  
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