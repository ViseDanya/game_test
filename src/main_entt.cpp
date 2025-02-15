#include "SDL3/SDL.h"

#include "imgui.h"
#include "imgui_impl_sdl3.h"
#include "imgui_impl_sdlrenderer3.h"

#include "constants.h"
#include "Draw.h"
#include "Player.h"
#include "platform.h"
#include "Stage.hpp"
#include "Client.hpp"
#include "ColorRenderer.h"
#include "PlayerInputController.h"
#include "TextureManager.h"
#include <iostream>
#include <entt/entt.hpp>

SDL_Window* window = nullptr;
SDL_Renderer* renderer = nullptr;

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

  window = SDL_CreateWindow("Fractals", WINDOW_WIDTH, WINDOW_HEIGHT, 0);
  if (window == NULL)
  {
    SDL_LogError(SDL_LOG_CATEGORY_ERROR, "SDL_CreateWindow failed: %s", SDL_GetError());
    SDL_Quit();
    exit(1);
  }

  renderer = SDL_CreateRenderer(window, nullptr);
  if (renderer == nullptr){
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

  ImGui_ImplSDL3_InitForSDLRenderer(window, renderer);
  ImGui_ImplSDLRenderer3_Init(renderer);
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
    ImGui_ImplSDLRenderer3_RenderDrawData(ImGui::GetDrawData(), renderer);
}

void Cleanup()
{
  ImGui_ImplSDL3_Shutdown();
  ImGui_ImplSDLRenderer3_Shutdown();
  ImGui::DestroyContext();
  SDL_DestroyWindow(window);
  SDL_Quit();
}

struct Box
{
    aabb box;
};

struct Velocity
{
    float x;
    float y;
};

struct InputControllerE
{
    SDL_Scancode left_key;
    SDL_Scancode right_key;
    SDL_Scancode up_key;
    SDL_Scancode down_key;
    bool gravity_enabled;
};

void applyInputToVelocity(entt::registry& registry, const bool* keystate)
{
    auto view = registry.view<Velocity, const InputControllerE>();
    view.each([&](Velocity& velocity, const InputControllerE& inputController) 
        {
            velocity.x = 0;
            velocity.y = 0;
            if (keystate[inputController.left_key])
            {
                velocity.x -= PLAYER_SPEED * 1.f/FPS;
            }
            if (keystate[inputController.right_key])
            {
                velocity.x += PLAYER_SPEED * 1.f/FPS;
            }
            if (keystate[inputController.up_key])
            {
                velocity.y += PLAYER_SPEED * 1.f/FPS;
            }
            if (keystate[inputController.down_key])
            {
                velocity.y -= PLAYER_SPEED * 1.f/FPS;
            }
    });
};

void applyVelocityToPosition(entt::registry& registry)
{
    auto view = registry.view<const Velocity, Box>();
    view.each([&](const Velocity& velocity, Box& box) 
        {
            box.box.center.x += velocity.x;
            box.box.center.y += velocity.y;
        });
}

void renderColoredEntites(entt::registry& registry, Draw drawer)
{
    auto view = registry.view<const Box, const SDL_Color>();
    view.each([&](const Box& box, const SDL_Color& color) 
        {
            drawer.renderColoredRectangle(color, box.box);
        });
};

int main(int argc, char *argv[])
{
  Init_SDL();
  Init_ImGui();
  Init_Enet();
  
  TextureManager textureManager(renderer);
  textureManager.loadAllTextures();

  Draw drawer(renderer);

  bool quit = false;
  SDL_Event event;

  entt::registry registry;
  const entt::entity p1Entity = registry.create();
  registry.emplace<Box>(p1Entity, aabb(vec2(WINDOW_WIDTH/2, WINDOW_HEIGHT/2), vec2(16, 16)));
  registry.emplace<SDL_Color>(p1Entity, PLAYER_ONE_COLOR);
  registry.emplace<Velocity>(p1Entity);
  InputControllerE& p1InputController = registry.emplace<InputControllerE>(p1Entity);
  p1InputController.left_key = SDL_SCANCODE_A;
  p1InputController.right_key = SDL_SCANCODE_D;
  p1InputController.up_key = SDL_SCANCODE_W;
  p1InputController.down_key = SDL_SCANCODE_S;


  const entt::entity p2Entity = registry.create();
  registry.emplace<Box>(p2Entity, aabb(vec2(WINDOW_WIDTH/4, WINDOW_HEIGHT/4), vec2(16, 16)));
  registry.emplace<SDL_Color>(p2Entity, PLAYER_TWO_COLOR);
  registry.emplace<Velocity>(p2Entity);
  InputControllerE& p2InputController = registry.emplace<InputControllerE>(p2Entity);
  p2InputController.left_key = SDL_SCANCODE_LEFT;
  p2InputController.right_key = SDL_SCANCODE_RIGHT;
  p2InputController.up_key = SDL_SCANCODE_UP;
  p2InputController.down_key = SDL_SCANCODE_DOWN;


  Player p1(vec2(WINDOW_WIDTH/2, WINDOW_HEIGHT/2));

  PlayerInputController playerOneController(p1);
  playerOneController.left_key = SDL_SCANCODE_A;
  playerOneController.right_key = SDL_SCANCODE_D;
  playerOneController.up_key = SDL_SCANCODE_W;
  playerOneController.down_key = SDL_SCANCODE_S;

  Player p2(vec2(WINDOW_WIDTH/4, WINDOW_HEIGHT/4));

  PlayerInputController playerTwoController(p2);
  playerTwoController.left_key = SDL_SCANCODE_LEFT;
  playerTwoController.right_key = SDL_SCANCODE_RIGHT;
  playerTwoController.up_key = SDL_SCANCODE_UP;
  playerTwoController.down_key = SDL_SCANCODE_DOWN;

  Stage stage;
  stage.addPlayer(&p1);
  stage.addPlayer(&p2);

  platform pform(vec2(WINDOW_WIDTH/2, WINDOW_HEIGHT/2 - PLAYER_HEIGHT * 2));
  stage.addPlatform(&pform);

  // ColorRenderer debugRenderer;

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

    playerOneController.gravity_enabled = gravity_enabled;
    playerOneController.ProcessInput(keystate);
    playerTwoController.gravity_enabled = gravity_enabled;
    playerTwoController.ProcessInput(keystate);

    stage.CollidePlayers();

    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);

    Show_ImGui();

    // debugRenderer.color = PLAYER_ONE_COLOR;
    // debugRenderer.render(p1.box);

    // debugRenderer.color = PLAYER_TWO_COLOR;
    // debugRenderer.render(p2.box);

    // debugRenderer.color = PLATFORM_COLOR;
    // for(const platform* p : stage.getPlatforms())
    // {
    //   debugRenderer.render(p->box);
    // }

    // drawer.renderColoredRectangle(PLAYER_ONE_COLOR, p1.box);
    // drawer.renderColoredRectangle(PLAYER_TWO_COLOR, p2.box);

    applyInputToVelocity(registry, keystate);
    applyVelocityToPosition(registry);
    renderColoredEntites(registry, drawer);

    for(const platform* p : stage.getPlatforms())
    {
      drawer.renderTexture(textureManager.normalTexture, p->box);
    }

    SDL_RenderPresent(renderer);

    Uint64 frameEndTime = SDL_GetTicks();
    Uint64 elapsedTime = frameEndTime - frameStartTime;
    if(elapsedTime < 1000/FPS)
    {
      SDL_Delay(1000/FPS - elapsedTime);
    }
  }

  // Client client;
  // client.ConnectToServer("localhost");
  // PlayerPo

  // SDL_Delay(5000);

  Cleanup();
}
