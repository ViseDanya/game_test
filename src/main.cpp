#include "SDL3/SDL.h"

#include "imgui.h"
#include "imgui_impl_sdl3.h"
#include "imgui_impl_sdlrenderer3.h"

#include "constants.h"
#include "player.h"
#include "platform.h"

#include <numeric>

SDL_Window* window = nullptr;
SDL_Renderer* renderer = nullptr;

std::vector<player*> players;
std::vector<platform*> platforms;

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

void CollideDynamicWithStaticVertical()
{
  bool verticalCollisionDetected;
  do
  {
      verticalCollisionDetected = false;
      for(dynamic_object* player : players)
      {
          for (game_object* platform : platforms)
          {
              if (player->box.intersects(platform->box))
              {
                  verticalCollisionDetected = true;
                  float dynamicStaticOverlap = player->box.top() - platform->box.bottom();
                  float staticDynamicOverlap = platform->box.top() - player->box.bottom();
                  if (dynamicStaticOverlap < staticDynamicOverlap)
                  {
                      player->is_on_ceiling = true;
                      player->box.center -= vec2::up * (dynamicStaticOverlap + COLLISION_TOLERANCE);
                      std::vector<dynamic_object*> downObjects = player->get_adjacency_list(direction::DOWN);
                      for (dynamic_object* obj : downObjects)
                      {
                          obj->is_on_ceiling = true;
                          obj->box.center -= vec2::up * dynamicStaticOverlap;
                      }
                  }
                  else
                  {
                      player->is_on_floor = true;
                      player->box.center += vec2::up * (staticDynamicOverlap + COLLISION_TOLERANCE);
                      std::vector<dynamic_object*> upObjects = player->get_adjacency_list(direction::UP);
                      for (dynamic_object* obj : upObjects)
                      {
                          obj->is_on_floor = true;
                          obj->box.center += vec2::up * staticDynamicOverlap;
                      }

                      // platform.HandleCollision(player);
                  }
              }
          }
      }
  } while (verticalCollisionDetected);
}

void CollideDynamicWithStaticHorizontal()
{
    bool horizontalCollisionDetected;
    do
    {
        horizontalCollisionDetected = false;
        for(dynamic_object* player : players)
        {
          for (game_object* platform : platforms)
            {
              if (player->box.intersects(platform->box))
                {
                    horizontalCollisionDetected = true;
                    float dynamicStaticOverlap = player->box.right() - platform->box.left();
                    float staticDynamicOverlap = platform->box.right() - player->box.left();
                    if (dynamicStaticOverlap < staticDynamicOverlap)
                    {
                        player->is_on_wall_right = true;
                        player->box.center -= vec2::right * (dynamicStaticOverlap + COLLISION_TOLERANCE);
                        std::vector<dynamic_object*> leftObjects = player->get_adjacency_list(direction::LEFT);
                        for (dynamic_object* obj : leftObjects)
                        {
                            obj->is_on_wall_right = true;
                            obj->box.center -= vec2::right * dynamicStaticOverlap;
                        }
                    }
                    else
                    {
                        player->is_on_wall_left = true;
                        player->box.center += vec2::right * (staticDynamicOverlap + COLLISION_TOLERANCE);
                        std::vector<dynamic_object*> rightObjects = player->get_adjacency_list(direction::RIGHT);
                        for (dynamic_object* obj : rightObjects)
                        {
                            obj->is_on_wall_left = true;
                            obj->box.center += vec2::right * staticDynamicOverlap;
                        }
                    }
                }
            }
        }

    } while (horizontalCollisionDetected);
}

void CollideDynamicWithDynamicVertical()
{
    bool verticalCollisionDetected;
    do
    {
        verticalCollisionDetected = false;
        for (int i = 0; i < players.size()-1; i++)
        {
            for (int j = i+1; j < players.size(); j++)
            {
                dynamic_object* obj1 = players[i];
                dynamic_object* obj2 = players[j];
                if (obj1->box.intersects(obj2->box))
                {
                    verticalCollisionDetected = true;
                    dynamic_object* downObject;
                    dynamic_object* upObject;
                    if (obj1->box.top() - obj2->box.bottom() < obj2->box.top() - obj1->box.bottom())
                    {
                        downObject = obj1;
                        upObject = obj2;
                    }
                    else
                    {
                        downObject = obj2;
                        upObject = obj1;
                    }

                    float overlap = downObject->box.top() - upObject->box.bottom();

                    std::vector<dynamic_object*> downObjects = downObject->get_adjacency_list(direction::DOWN);
                    std::vector<dynamic_object*> upObjects = upObject->get_adjacency_list(direction::UP);

                    if (downObject->is_on_floor)
                    {
                        upObject->is_on_floor = true;
                        upObject->box.center += vec2::up * (overlap + COLLISION_TOLERANCE);
                        for(dynamic_object* obj : upObjects)
                        {
                            upObject->is_on_floor = true;
                            upObject->box.center += vec2::up * overlap;
                        }
                    }
                    else if(upObject->is_on_ceiling)
                    {
                        downObject->is_on_ceiling = true;
                        downObject->box.center -= vec2::up * (overlap + COLLISION_TOLERANCE);
                        for(dynamic_object* obj : upObjects)
                        {
                            downObject->is_on_ceiling = true;
                            downObject->box.center -= vec2::up * overlap;
                        }
                    }
                    else
                    {
                        //float massDown = downObject->mass + downObjects.Aggregate(0.0f, (acc, obj) => acc + obj.mass);
                        //float massUp = upObject.mass + upObjects.Aggregate(0.0f, (acc, obj) => acc + obj.mass);
                        //float totalMass = massDown + massUp;

                        //downObject->box.center -= vec2::up * (overlap * massUp / totalMass + COLLISION_TOLERANCE);
                        //upObject->box.center += vec2::up * (overlap * massDown / totalMass + COLLISION_TOLERANCE);

                        //downObjects.ForEach(obj => obj->box.center -= vec2::up * (overlap * massUp / totalMass));
                        //upObjects.ForEach(obj => obj->box.center += vec2::up * (overlap * massDown / totalMass));
                        downObject->box.center -= vec2::up * (overlap + COLLISION_TOLERANCE);
                        upObject->velocity.y = downObject->velocity.y;
                        downObject->velocity.y = 0;
                    }

                    downObject->adjacencies[direction::UP] = upObject;
                    upObject->adjacencies[direction::DOWN] = downObject;
                }
            }
        }
    } while (verticalCollisionDetected);
}

void CollideDynamicWithDynamicHorizontal()
{
    bool horizontalCollisionDetected;
    do
    {
        horizontalCollisionDetected = false;
        for (int i = 0; i < players.size()-1; i++)
        {
            for (int j = i+1; j < players.size(); j++)
            {
                dynamic_object* obj1 = players[i];
                dynamic_object* obj2 = players[j];
                if (obj1->box.intersects(obj2->box))
                {
                    horizontalCollisionDetected = true;
                    dynamic_object* leftObject;
                    dynamic_object* rightObject;
                    if (obj1->box.right() - obj2->box.left() < obj2->box.right() - obj1->box.left())
                    {
                        leftObject = obj1;
                        rightObject = obj2;
                    }
                    else
                    {
                        leftObject = obj2;
                        rightObject = obj1;
                    }

                    float overlap = leftObject->box.right() - rightObject->box.left();

                    std::vector<dynamic_object*> leftObjects = leftObject->get_adjacency_list(direction::LEFT);
                    std::vector<dynamic_object*> rightObjects = rightObject->get_adjacency_list(direction::RIGHT);

                    float massLeft = std::accumulate(leftObjects.cbegin(), leftObjects.cend(), leftObject->mass, 
                      [&](float acc, dynamic_object* obj){return acc + obj->mass;});
                    float massRight = std::accumulate(rightObjects.cbegin(), rightObjects.cend(), rightObject->mass, 
                      [&](float acc, dynamic_object* obj){return acc + obj->mass;});
                    float totalMass = massLeft + massRight;

                    leftObject->box.center -= vec2::right * (overlap * massRight / totalMass + COLLISION_TOLERANCE);
                    rightObject->box.center += vec2::right * (overlap * massLeft / totalMass + COLLISION_TOLERANCE);

                    std::for_each(leftObjects.cbegin(), leftObjects.cend(), [&](dynamic_object* obj){obj->box.center -= vec2::right * (overlap * massRight / totalMass);});
                    std::for_each(rightObjects.cbegin(), rightObjects.cend(), [&](dynamic_object* obj){obj->box.center -= vec2::right * (overlap * massLeft / totalMass);});

                    leftObject->adjacencies[direction::RIGHT] = rightObject;
                    rightObject->adjacencies[direction::LEFT] = leftObject;
                }
            }
        }
    } while (horizontalCollisionDetected);
}

void CollidePlayers()
{
  for(player* p : players)
  {
    p->reset();
    p->box.center += p->velocity.y * 1.f/FPS * vec2::up;
  }
  CollideDynamicWithStaticVertical();
  CollideDynamicWithDynamicVertical();
  CollideDynamicWithStaticVertical();

  for(player* p : players)
  {
    p->box.center += p->velocity.x * 1.f/FPS * vec2::right;
  }
  CollideDynamicWithStaticHorizontal();
  CollideDynamicWithDynamicHorizontal();
  CollideDynamicWithStaticHorizontal();
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

  players.push_back(&p1);
  players.push_back(&p2);

  platform pform(vec2(WINDOW_WIDTH/2, WINDOW_HEIGHT/2 - PLAYER_HEIGHT * 2));
  platforms.push_back(&pform);

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
    std::for_each(players.cbegin(), players.cend(), [&](player* p){p->process_input(keystate, gravity_enabled);});

    CollidePlayers();

    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);

    Show_ImGui();
    for(player* p : players)
    {
      p->render();
    }
    for(platform* p : platforms)
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
