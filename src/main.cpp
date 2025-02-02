#include "SDL3/SDL.h"

#include "imgui.h"
#include "imgui_impl_sdl3.h"
#include "imgui_impl_sdlrenderer3.h"

#include "constants.h"
#include "player.h"
#include "platform.h"
#include "Stage.hpp"

SDL_Window* window = nullptr;
SDL_Renderer* renderer = nullptr;

static bool gravity_enabled = false;

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

int main(int argc, char *argv[])
{
  Init_SDL();
  Init_ImGui();

  bool quit = false;
  SDL_Event event;

  player p1(vec2(WINDOW_WIDTH/2, WINDOW_HEIGHT/2));
  p1.left_key = SDL_SCANCODE_A;
  p1.right_key = SDL_SCANCODE_D;
  p1.up_key = SDL_SCANCODE_W;
  p1.down_key = SDL_SCANCODE_S;

  player p2(vec2(WINDOW_WIDTH/4, WINDOW_HEIGHT/4));
  p2.left_key = SDL_SCANCODE_LEFT;
  p2.right_key = SDL_SCANCODE_RIGHT;
  p2.up_key = SDL_SCANCODE_UP;
  p2.down_key = SDL_SCANCODE_DOWN;

  Stage stage;
  stage.addPlayer(&p1);
  stage.addPlayer(&p2);

  platform pform(vec2(WINDOW_WIDTH/2, WINDOW_HEIGHT/2 - PLAYER_HEIGHT * 2));
  stage.addPlatform(&pform);

  bool gravity = false;

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
    std::for_each(stage.getPlayers().cbegin(), stage.getPlayers().cend(), [&](player* p){p->process_input(keystate, gravity_enabled);});

    stage.CollidePlayers();

    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);

    Show_ImGui();
    for(const player* p : stage.getPlayers())
    {
      p->render();
    }
    for(const platform* p : stage.getPlatforms())
    {
      p->render();
    }

    SDL_RenderPresent(renderer);

    Uint64 frameEndTime = SDL_GetTicks();
    Uint64 elapsedTime = frameEndTime - frameStartTime;
    if(elapsedTime < 1000/FPS)
    {
      SDL_Delay(1000/FPS - elapsedTime);
    }
  }

  Cleanup();
}
