#include "SDL3/SDL.h"

#include "imgui.h"
#include "imgui_impl_sdl3.h"
#include "imgui_impl_sdlrenderer3.h"

#include "Constants.h"
#include "Renderer.h"
#include "Client.hpp"
#include "TextureManager.h"
#include "Components/Box.h"
#include "Components/Velocity.h"
#include "Components/Adjacencies.h"
#include "Components/Mass.h"
#include <iostream>
#include <entt/entt.hpp>
#include <numeric>
#include "Collision.h"

static SDL_Window* window = nullptr;
static SDL_Renderer* sdlRenderer = nullptr;

static bool gravity_enabled = false;

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
    ImGui::Checkbox("Gravity", &gravity_enabled);
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

struct InputController
{
    SDL_Scancode left_key;
    SDL_Scancode right_key;
    SDL_Scancode up_key;
    SDL_Scancode down_key;
    bool gravity_enabled;
};

struct Sprite
{
    SDL_Texture* texture;
};

void applyGravityToVelocity(entt::registry& registry)
{
  auto view = registry.view<Velocity>();
  view.each([&](Velocity& velocity) 
  {
    velocity.velocity.y += GRAVITY * 1.f/FPS * 1.f/FPS;
  });
}

void applyInputToVelocity(entt::registry& registry, const bool* keystate, const bool gravity_enabled)
{
    auto view = registry.view<Velocity, const InputController, const Adjacencies>();
    view.each([&](Velocity& velocity, const InputController& inputController, const Adjacencies& adjacencies) 
        {
            velocity.velocity.x = 0;

            if (keystate[inputController.left_key])
            {
                velocity.velocity.x -= PLAYER_SPEED * 1.f/FPS;
            }
            if (keystate[inputController.right_key])
            {
                velocity.velocity.x += PLAYER_SPEED * 1.f/FPS;
            }
            if (keystate[inputController.up_key])
            {
              if(!gravity_enabled)
              {
                velocity.velocity.y += PLAYER_SPEED * 1.f/FPS;
              }
              else if(adjacencies.isOnFloor)
              {
                velocity.velocity.y += PLAYER_JUMP_SPEED * 1.f/FPS;
              }
            }
            if (keystate[inputController.down_key] && !gravity_enabled)
            {
                velocity.velocity.y -= PLAYER_SPEED * 1.f/FPS;
            }
    });
};

void applyVelocityToPosition(entt::registry& registry)
{
    auto view = registry.view<const Velocity, Box>();
    view.each([&](const Velocity& velocity, Box& box) 
        {
            box.center += velocity.velocity;
        });
}

void renderColoredEntities(entt::registry& registry, Renderer& renderer)
{
    auto view = registry.view<const Box, const SDL_Color>();
    view.each([&](const Box& box, const SDL_Color& color) 
        {
          renderer.renderColoredRectangle(color, box);
        });
};

void renderSprites(entt::registry& registry, Renderer& renderer)
{
    auto view = registry.view<const Box, const Sprite>();
    view.each([&](const Box& box, const Sprite& sprite) 
        {
          renderer.renderTexture(sprite.texture, box);
        });
};

void resetVerticalVelocityForEntitiesOnFloor(entt::registry& registry)
{
  auto view = registry.view<Velocity, const Adjacencies>();
  view.each([&](Velocity& velocity, const Adjacencies& adjacencies) 
  {
    if(adjacencies.isOnFloor)
    {
      velocity.velocity.y = 0;
    }
  });
}

void resetAdjacencies(entt::registry& registry)
{
  auto adjacencies = registry.view<Adjacencies>();
  adjacencies.each([&](Adjacencies& adjacencies) 
  {
    adjacencies.reset();
  });
}

int main(int argc, char *argv[])
{
  Init_SDL();
  Init_ImGui();
  Init_Enet();
  
  TextureManager textureManager(sdlRenderer);
  textureManager.loadAllTextures();

  Renderer drawer(sdlRenderer);

  bool quit = false;
  SDL_Event event;

  entt::registry registry;
  const entt::entity p1Entity = registry.create();
  registry.emplace<Box>(p1Entity, Box(glm::vec2(WINDOW_WIDTH/2, WINDOW_HEIGHT/2), glm::vec2(16, 16)));
  registry.emplace<SDL_Color>(p1Entity, PLAYER_ONE_COLOR);
  registry.emplace<Velocity>(p1Entity);
  registry.emplace<Mass>(p1Entity, 1.);
  registry.emplace<Adjacencies>(p1Entity);
  InputController& p1InputController = registry.emplace<InputController>(p1Entity);
  p1InputController.left_key = SDL_SCANCODE_A;
  p1InputController.right_key = SDL_SCANCODE_D;
  p1InputController.up_key = SDL_SCANCODE_W;
  p1InputController.down_key = SDL_SCANCODE_S;

  const entt::entity p2Entity = registry.create();
  registry.emplace<Box>(p2Entity, Box(glm::vec2(WINDOW_WIDTH/4, WINDOW_HEIGHT/4), glm::vec2(16, 16)));
  registry.emplace<SDL_Color>(p2Entity, PLAYER_TWO_COLOR);
  registry.emplace<Velocity>(p2Entity);
  registry.emplace<Mass>(p2Entity, 1.);
  registry.emplace<Adjacencies>(p2Entity);
  InputController& p2InputController = registry.emplace<InputController>(p2Entity);
  p2InputController.left_key = SDL_SCANCODE_LEFT;
  p2InputController.right_key = SDL_SCANCODE_RIGHT;
  p2InputController.up_key = SDL_SCANCODE_UP;
  p2InputController.down_key = SDL_SCANCODE_DOWN;

  const entt::entity p3Entity = registry.create();
  registry.emplace<Box>(p3Entity, Box(glm::vec2(WINDOW_WIDTH/8, WINDOW_HEIGHT/8), glm::vec2(16, 16)));
  registry.emplace<SDL_Color>(p3Entity, PLAYER_THREE_COLOR);
  registry.emplace<Velocity>(p3Entity);
  registry.emplace<Mass>(p3Entity, 1.);
  registry.emplace<Adjacencies>(p3Entity);
  InputController& p3InputController = registry.emplace<InputController>(p3Entity);
  p3InputController.left_key = SDL_SCANCODE_J;
  p3InputController.right_key = SDL_SCANCODE_L;
  p3InputController.up_key = SDL_SCANCODE_I;
  p3InputController.down_key = SDL_SCANCODE_K;

  const entt::entity boxEntity = registry.create();
  registry.emplace<Box>(boxEntity, Box(glm::vec2(WINDOW_WIDTH/2, WINDOW_HEIGHT/2), glm::vec2(8, 8)));
  registry.emplace<SDL_Color>(boxEntity, BOX_COLOR);
  registry.emplace<Velocity>(boxEntity);
  registry.emplace<Mass>(boxEntity, 1.);
  registry.emplace<Adjacencies>(boxEntity);

  const entt::entity platformEntity = registry.create();
  registry.emplace<Box>(platformEntity, Box(glm::vec2(WINDOW_WIDTH/2, WINDOW_HEIGHT/2 - PLAYER_HEIGHT * 2), glm::vec2(48, 8)));
  registry.emplace<Sprite>(platformEntity, textureManager.normalTexture);
  registry.emplace<Conveyor>(platformEntity);

  const entt::entity floor = registry.create();
  registry.emplace<Box>(floor, Box(glm::vec2(WINDOW_WIDTH/2, 0), glm::vec2(WINDOW_WIDTH/2, 8)));
  registry.emplace<Sprite>(floor, textureManager.normalTexture);
  registry.emplace<Trampoline>(floor);

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
    }

    const bool *keystate = SDL_GetKeyboardState(nullptr);

    SDL_SetRenderDrawColor(sdlRenderer, 0, 0, 0, 255);
    SDL_RenderClear(sdlRenderer);

    Show_ImGui();

    resetVerticalVelocityForEntitiesOnFloor(registry);

    if(gravity_enabled)
    {
      applyGravityToVelocity(registry);
    }

    applyInputToVelocity(registry, keystate, gravity_enabled);
    resetAdjacencies(registry);
    applyVelocityToPosition(registry);
    resolveCollisions(registry);

    renderColoredEntities(registry, drawer);
    renderSprites(registry, drawer);

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
