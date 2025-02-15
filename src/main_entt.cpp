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

struct Sprite
{
    SDL_Texture* texture;
};

struct Mass
{
    float mass;
};

struct Adjacencies
{
    bool is_on_ceiling;
    bool is_on_floor;
    bool is_on_wall_left;
    bool is_on_wall_right;
    std::unordered_map<direction, entt::entity> adjacencies;

    std::vector<entt::entity> get_adjacency_list(entt::registry& registry, direction dir)
    {
        std::vector<entt::entity> adjacency_list;
        std::unordered_map<direction, entt::entity> currentAdjacencies = adjacencies;
        while(currentAdjacencies.find(dir) != currentAdjacencies.end())
        {
            entt::entity currentEntity = currentAdjacencies[dir];
            currentAdjacencies = registry.get<Adjacencies>(currentEntity).adjacencies;
            adjacency_list.push_back(currentEntity);
        }
        return adjacency_list;
    }
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

void applyVerticalVelocityToPosition(entt::registry& registry)
{
    auto view = registry.view<const Velocity, Box>();
    view.each([&](const Velocity& velocity, Box& box) 
        {
            box.box.center.y += velocity.y;
        });
}

void applyHorizontalVelocityToPosition(entt::registry& registry)
{
    auto view = registry.view<const Velocity, Box>();
    view.each([&](const Velocity& velocity, Box& box) 
        {
            box.box.center.x += velocity.x;
        });
}

void renderColoredEntities(entt::registry& registry, Draw& drawer)
{
    auto view = registry.view<const Box, const SDL_Color>();
    view.each([&](const Box& box, const SDL_Color& color) 
        {
            drawer.renderColoredRectangle(color, box.box);
        });
};

void renderSprites(entt::registry& registry, Draw& drawer)
{
    auto view = registry.view<const Box, const Sprite>();
    view.each([&](const Box& box, const Sprite& sprite) 
        {
            drawer.renderTexture(sprite.texture, box.box);
        });
};


void collideDynamicEntitiesVertically(entt::registry& registry)
{
    auto dynamicEntities = registry.view<Box, const Velocity, const Mass, Adjacencies>();
    auto collidableEntities = registry.view<Box>();
    dynamicEntities.each([&](const entt::entity e1, Box& box1, const Velocity& velocity, const Mass mass, Adjacencies& adjacencies) 
    {
        collidableEntities.each([&](const entt::entity e2, Box& box2)
        {
            if (e1 != e2 && box1.box.intersects(box2.box))
              {
                  float dynamicStaticOverlap = box1.box.top() - box2.box.bottom();
                  float staticDynamicOverlap = box2.box.top() - box1.box.bottom();
                  if (dynamicStaticOverlap < staticDynamicOverlap)
                  {
                      adjacencies.is_on_ceiling = true;
                      box1.box.center -= vec2::up * (dynamicStaticOverlap + COLLISION_TOLERANCE);
                    //   std::vector<entt::entity> downObjects = adjacencies.get_adjacency_list(registry, direction::DOWN);
                    //   for (entt::entity obj : downObjects)
                    //   {
                    //       Adjacencies& objAjacencies = registry.get<Adjacencies>(obj);
                    //       objAjacencies.is_on_ceiling = true;
                    //       Box& objBox = registry.get<Box>(obj);
                    //       objBox.box.center -= vec2::up * dynamicStaticOverlap;
                    //   }
                  }
                  else
                  {
                      adjacencies.is_on_floor = true;
                      box1.box.center += vec2::up * (staticDynamicOverlap + COLLISION_TOLERANCE);
                    //   std::vector<entt::entity> upObjects = adjacencies.get_adjacency_list(registry, direction::UP);
                    //   for (entt::entity obj : upObjects)
                    //   {
                    //     Adjacencies& objAjacencies = registry.get<Adjacencies>(obj);
                    //       objAjacencies.is_on_floor = true;
                    //       Box& objBox = registry.get<Box>(obj);
                    //       objBox.box.center += vec2::up * staticDynamicOverlap;
                    //   }
                    }
                }
                      
        });
    });
}

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
  registry.emplace<Mass>(p1Entity, 1.);
  registry.emplace<Adjacencies>(p1Entity);
  InputControllerE& p1InputController = registry.emplace<InputControllerE>(p1Entity);
  p1InputController.left_key = SDL_SCANCODE_A;
  p1InputController.right_key = SDL_SCANCODE_D;
  p1InputController.up_key = SDL_SCANCODE_W;
  p1InputController.down_key = SDL_SCANCODE_S;


  const entt::entity p2Entity = registry.create();
  registry.emplace<Box>(p2Entity, aabb(vec2(WINDOW_WIDTH/4, WINDOW_HEIGHT/4), vec2(16, 16)));
  registry.emplace<SDL_Color>(p2Entity, PLAYER_TWO_COLOR);
  registry.emplace<Velocity>(p2Entity);
  registry.emplace<Mass>(p2Entity, 1.);
  registry.emplace<Adjacencies>(p2Entity);
  InputControllerE& p2InputController = registry.emplace<InputControllerE>(p2Entity);
  p2InputController.left_key = SDL_SCANCODE_LEFT;
  p2InputController.right_key = SDL_SCANCODE_RIGHT;
  p2InputController.up_key = SDL_SCANCODE_UP;
  p2InputController.down_key = SDL_SCANCODE_DOWN;

  const entt::entity platformEntity = registry.create();
  registry.emplace<Box>(platformEntity, aabb(vec2(WINDOW_WIDTH/2, WINDOW_HEIGHT/2 - PLAYER_HEIGHT * 2), vec2(48, 8)));
  registry.emplace<Sprite>(platformEntity, textureManager.normalTexture);

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

    applyVerticalVelocityToPosition(registry);
    collideDynamicEntitiesVertically(registry);
    applyHorizontalVelocityToPosition(registry);

    renderColoredEntities(registry, drawer);
    renderSprites(registry, drawer);

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
