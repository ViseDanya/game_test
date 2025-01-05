#include "SDL3/SDL.h"

#include "imgui.h"
#include "imgui_impl_sdl3.h"
#include "imgui_impl_sdlrenderer3.h"

#include "constants.h"
#include "player.h"
#include "platform.h"

SDL_Window* window = nullptr;
SDL_Renderer* renderer = nullptr;

std::vector<player*> players;
std::vector<platform*> platforms;

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
    ImGui::SetNextWindowSize({300, 200});
    ImGui::SetNextWindowPos({10, 10});
    ImGui::Begin("Settings");
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
                      player->isOnCeiling = true;
                      player->box.center -= vec2::up * (dynamicStaticOverlap + COLLISION_TOLERANCE);
                      std::vector<dynamic_object*> downObjects = player->get_adjacency_list(direction::DOWN);
                      for (dynamic_object* obj : downObjects)
                      {
                          obj->isOnCeiling = true;
                          obj->box.center -= vec2::up * dynamicStaticOverlap;
                      }
                  }
                  else
                  {
                      player->isOnFloor = true;
                      player->box.center += vec2::up * (staticDynamicOverlap + COLLISION_TOLERANCE);
                      std::vector<dynamic_object*> upObjects = player->get_adjacency_list(direction::UP);
                      for (dynamic_object* obj : upObjects)
                      {
                          obj->isOnFloor = true;
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
                        player->isOnWallRight = true;
                        player->box.center -= vec2::right * (dynamicStaticOverlap + COLLISION_TOLERANCE);
                        std::vector<dynamic_object*> leftObjects = player->get_adjacency_list(direction::LEFT);
                        for (dynamic_object* obj : leftObjects)
                        {
                            obj->isOnWallRight = true;
                            obj->box.center -= vec2::right * dynamicStaticOverlap;
                        }
                    }
                    else
                    {
                        player->isOnWallLeft = true;
                        player->box.center += vec2::right * (staticDynamicOverlap + COLLISION_TOLERANCE);
                        std::vector<dynamic_object*> rightObjects = player->get_adjacency_list(direction::RIGHT);
                        for (dynamic_object* obj : rightObjects)
                        {
                            obj->isOnWallLeft = true;
                            obj->box.center += vec2::right * staticDynamicOverlap;
                        }
                    }
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
    p->box.center += p->velocity.y * vec2::up;
  }
  CollideDynamicWithStaticVertical();
  // CollideDynamicWithDynamicVertical();
  // CollideDynamicWithStaticVertical();

  for(player* p : players)
  {
    p->box.center += p->velocity.x * vec2::right;
  }
  CollideDynamicWithStaticHorizontal();
  // CollideDynamicWithDynamicHorizontal();
  // CollideDynamicWithStaticHorizontal();
}

int main(int argc, char *argv[])
{
  Init_SDL();
  Init_ImGui();

  bool quit = false;
  SDL_Event event;

  player p(vec2(WINDOW_WIDTH/2, WINDOW_HEIGHT/2));
  players.push_back(&p);
  platform pform(vec2(WINDOW_WIDTH/2, WINDOW_HEIGHT/2 - PLAYER_HEIGHT * 2));
  platforms.push_back(&pform);

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

    p.velocity = vec2::zero;
    const bool *keystate = SDL_GetKeyboardState(nullptr);
    if(keystate[SDL_SCANCODE_A])
    {
      p.velocity.x -= PLAYER_SPEED;
    }
    if(keystate[SDL_SCANCODE_D])
    {
      p.velocity.x += PLAYER_SPEED;
    }
    if(keystate[SDL_SCANCODE_W])
    {
      p.velocity.y += PLAYER_SPEED;
    }
    if(keystate[SDL_SCANCODE_S])
    {
      p.velocity.y -= PLAYER_SPEED;
    }

    CollidePlayers();

    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);

    Show_ImGui();
    p.render();
    pform.render();

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

    // private void CollideDynamicWithDynamicVertical(playerect[] players)
    // {
    //     bool verticalCollisionDetected;
    //     do
    //     {
    //         verticalCollisionDetected = false;
    //         for (int i = 0; i < players.Length-1; i++)
    //         {
    //             for (int j = i+1; j < players.Length; j++)
    //             {
    //                 playerect obj1 = players[i];
    //                 playerect obj2 = players[j];
    //                 if (obj1.CollidesWith(obj2))
    //                 {
    //                     verticalCollisionDetected = true;
    //                     playerect downObject;
    //                     playerect upObject;
    //                     if (obj1->box.top() - obj2->box.bottom() < obj2->box.top() - obj1->box.bottom())
    //                     {
    //                         downObject = obj1;
    //                         upObject = obj2;
    //                     }
    //                     else
    //                     {
    //                         downObject = obj2;
    //                         upObject = obj1;
    //                     }

    //                     float overlap = downObject->box.top() - upObject->box.bottom();

    //                     List<playerect> downObjects = downObject.GetAdjacencyList(playerect.Direction.DOWN);
    //                     List<playerect> upObjects = upObject.GetAdjacencyList(playerect.Direction.UP);

    //                     if (downObject.isOnFloor)
    //                     {
    //                         upObject.isOnFloor = true;
    //                         upObject->box.center += vec2::up * (overlap + collisionTolerance);
    //                         foreach(playerect obj in upObjects)
    //                         {
    //                             upObject.isOnFloor = true;
    //                             upObject->box.center += vec2::up * overlap;
    //                         }
    //                     }
    //                     else if(upObject.isOnCeiling)
    //                     {
    //                         downObject.isOnCeiling = true;
    //                         downObject->box.center -= vec2::up * (overlap + collisionTolerance);
    //                         foreach (playerect obj in upObjects)
    //                         {
    //                             downObject.isOnCeiling = true;
    //                             downObject->box.center -= vec2::up * overlap;
    //                         }
    //                     }
    //                     else
    //                     {
    //                         //float massDown = downObject.mass + downObjects.Aggregate(0.0f, (acc, obj) => acc + obj.mass);
    //                         //float massUp = upObject.mass + upObjects.Aggregate(0.0f, (acc, obj) => acc + obj.mass);
    //                         //float totalMass = massDown + massUp;

    //                         //downObject->box.center -= vec2::up * (overlap * massUp / totalMass + collisionTolerance);
    //                         //upObject->box.center += vec2::up * (overlap * massDown / totalMass + collisionTolerance);

    //                         //downObjects.ForEach(obj => obj->box.center -= vec2::up * (overlap * massUp / totalMass));
    //                         //upObjects.ForEach(obj => obj->box.center += vec2::up * (overlap * massDown / totalMass));
    //                         downObject->box.center -= vec2::up * (overlap + collisionTolerance);
    //                         upObject.velocity.y = downObject.velocity.y;
    //                         Debug.Log(upObject.velocity);
    //                         downObject.velocity.y = 0;
    //                     }

    //                     downObject.Adjacencies[playerect.Direction.UP] = upObject;
    //                     upObject.Adjacencies[playerect.Direction.DOWN] = downObject;
    //                 }
    //             }
    //         }
    //     } while (verticalCollisionDetected);
    // }

    // private void CollideDynamicWithDynamicHorizontal(playerect[] players)
    // {
    //     bool horizontalCollisionDetected;
    //     do
    //     {
    //         horizontalCollisionDetected = false;
    //         for (int i = 0; i < players.Length-1; i++)
    //         {
    //             for (int j = i+1; j < players.Length; j++)
    //             {
    //                 playerect obj1 = players[i];
    //                 playerect obj2 = players[j];
    //                 if (obj1.CollidesWith(obj2))
    //                 {
    //                     horizontalCollisionDetected = true;
    //                     playerect left()Object;
    //                     playerect right()Object;
    //                     if (obj1->box.right() - obj2->box.left() < obj2->box.right() - obj1->box.left())
    //                     {
    //                         left()Object = obj1;
    //                         right()Object = obj2;
    //                     }
    //                     else
    //                     {
    //                         left()Object = obj2;
    //                         right()Object = obj1;
    //                     }

    //                     float overlap = left()Object->box.right() - right()Object->box.left();

    //                     List<playerect> left()Objects = left()Object.GetAdjacencyList(playerect.Direction.left());
    //                     List<playerect> right()Objects = right()Object.GetAdjacencyList(playerect.Direction.right());

    //                     float massleft() = left()Object.mass + left()Objects.Aggregate(0.0f, (acc, obj) => acc + obj.mass);
    //                     float massright() = right()Object.mass + right()Objects.Aggregate(0.0f, (acc, obj) => acc + obj.mass);
    //                     float totalMass = massleft() + massright();

    //                     left()Object->box.center -= vec2::right() * (overlap * massright() / totalMass + collisionTolerance);
    //                     right()Object->box.center += vec2::right() * (overlap * massleft() / totalMass + collisionTolerance);

    //                     left()Objects.ForEach(obj => obj->box.center -= vec2::right() * (overlap * massright() / totalMass));
    //                     right()Objects.ForEach(obj => obj->box.center += vec2::right() * (overlap * massleft() / totalMass));

    //                     left()Object.Adjacencies[playerect.Direction.right()] = right()Object;
    //                     right()Object.Adjacencies[playerect.Direction.left()] = left()Object;
    //                 }
    //             }
    //         }
    //     } while (horizontalCollisionDetected);
    // }
