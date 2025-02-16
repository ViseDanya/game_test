#include "SDL3/SDL.h"

#include "imgui.h"
#include "imgui_impl_sdl3.h"
#include "imgui_impl_sdlrenderer3.h"

#include "Constants.h"
#include "Renderer.h"
#include "Client.hpp"
#include "TextureManager.h"
#include <iostream>
#include <entt/entt.hpp>
#include <numeric>

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

struct Velocity
{
    glm::vec2 velocity;
};

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

struct Mass
{
    float mass;
};

enum Direction
{
  UP, DOWN, LEFT, RIGHT
};

struct Adjacencies
{
    bool is_on_ceiling;
    bool is_on_floor;
    bool is_on_wall_left;
    bool is_on_wall_right;
    std::unordered_map<Direction, entt::entity> adjacencies;

    void reset()
    {
      is_on_ceiling = false;
      is_on_floor = false;
      is_on_wall_left = false;
      is_on_wall_right = false;
      adjacencies.clear();  
    }
    
    std::vector<entt::entity> get_adjacency_list(entt::registry& registry, Direction dir)
    {
        std::vector<entt::entity> adjacency_list;
        std::unordered_map<Direction, entt::entity> currentAdjacencies = adjacencies;
        while(currentAdjacencies.find(dir) != currentAdjacencies.end())
        {
            entt::entity currentEntity = currentAdjacencies[dir];
            currentAdjacencies = registry.get<Adjacencies>(currentEntity).adjacencies;
            adjacency_list.push_back(currentEntity);
        }
        return adjacency_list;
    }
};

void applyInputToVelocity(entt::registry& registry, const bool* keystate, const bool gravity_enabled)
{
    auto view = registry.view<Velocity, const InputController, const Adjacencies>();
    view.each([&](Velocity& velocity, const InputController& inputController, const Adjacencies& adjacencies) 
        {
            velocity.velocity.x = 0;
            if (!gravity_enabled || adjacencies.is_on_floor)
            {
                velocity.velocity.y = 0;
            }
            else
            {
                velocity.velocity.y += GRAVITY * 1.f/FPS * 1.f/FPS;
            }

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
              else if(adjacencies.is_on_floor)
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

void applyVerticalVelocityToPosition(entt::registry& registry)
{
    auto view = registry.view<const Velocity, Box>();
    view.each([&](const Velocity& velocity, Box& box) 
        {
            box.center.y += velocity.velocity.y;
        });
}

void applyHorizontalVelocityToPosition(entt::registry& registry)
{
    auto view = registry.view<const Velocity, Box>();
    view.each([&](const Velocity& velocity, Box& box) 
        {
            box.center.x += velocity.velocity.x;
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

void collideDynamicWithStaticEntityVertically(entt::registry& registry, Box& box1, Adjacencies& adjacencies, Box& box2)
{
  float dynamicStaticOverlap = box1.top() - box2.bottom();
                float staticDynamicOverlap = box2.top() - box1.bottom();
                if (dynamicStaticOverlap < staticDynamicOverlap)
                {
                    adjacencies.is_on_ceiling = true;
                    box1.center -= glm::UP * (dynamicStaticOverlap + COLLISION_TOLERANCE);
                    std::vector<entt::entity> downObjects = adjacencies.get_adjacency_list(registry, Direction::DOWN);
                    for (entt::entity obj : downObjects)
                    {
                        Adjacencies& objAjacencies = registry.get<Adjacencies>(obj);
                        objAjacencies.is_on_ceiling = true;
                        Box& objBox = registry.get<Box>(obj);
                        objBox.center -= glm::UP * dynamicStaticOverlap;
                    }
                }
                else
                {
                    adjacencies.is_on_floor = true;
                    box1.center += glm::UP * (staticDynamicOverlap + COLLISION_TOLERANCE);
                    std::vector<entt::entity> upObjects = adjacencies.get_adjacency_list(registry, Direction::UP);
                    for (entt::entity obj : upObjects)
                    {
                      Adjacencies& objAjacencies = registry.get<Adjacencies>(obj);
                        objAjacencies.is_on_floor = true;
                        Box& objBox = registry.get<Box>(obj);
                        objBox.center += glm::UP * staticDynamicOverlap;
                    }
                  }
  }

void  collideDownWithUpDynamicEntityVertically(entt::registry& registry, entt::entity downObject, entt::entity upObject)
{
  auto [downBox, downAdjacencies, downMass] = registry.get<Box, Adjacencies, const Mass>(downObject);
  auto [upBox, upAdjacencies, upMass] = registry.get<Box, Adjacencies, const Mass>(upObject);

  float overlap = downBox.top() - upBox.bottom();

                    std::vector<entt::entity> downObjects = downAdjacencies.get_adjacency_list(registry, Direction::DOWN);
                    std::vector<entt::entity> upObjects = upAdjacencies.get_adjacency_list(registry, Direction::UP);

                    if (downAdjacencies.is_on_floor)
                    {
                        upAdjacencies.is_on_floor = true;
                        upBox.center += glm::UP * (overlap + COLLISION_TOLERANCE);
                        for(entt::entity obj : upObjects)
                        {
                          Adjacencies& objAjacencies = registry.get<Adjacencies>(obj);
                          objAjacencies.is_on_floor = true;
                          Box& objBox = registry.get<Box>(obj);
                          objBox.center += glm::UP * (overlap + COLLISION_TOLERANCE);
                        }
                    }
                    else if(upAdjacencies.is_on_ceiling)
                    {
                        downAdjacencies.is_on_ceiling = true;
                        downBox.center -= glm::UP * (overlap + COLLISION_TOLERANCE);
                        for(entt::entity obj : downObjects)
                        {
                          Adjacencies& objAjacencies = registry.get<Adjacencies>(obj);
                          objAjacencies.is_on_ceiling = true;
                          Box& objBox = registry.get<Box>(obj);
                          objBox.center -= glm::UP * (overlap + COLLISION_TOLERANCE);
                        }
                    }
                    else
                    {
                        //float massDown = downObject->mass + downObjects.Aggregate(0.0f, (acc, obj) => acc + obj.mass);
                        //float massUp = upObject.mass + upObjects.Aggregate(0.0f, (acc, obj) => acc + obj.mass);
                        //float totalMass = massDown + massUp;

                        //downBox.center -= glm::UP * (overlap * massUp / totalMass + COLLISION_TOLERANCE);
                        //upBox.center += glm::UP * (overlap * massDown / totalMass + COLLISION_TOLERANCE);

                        //downObjects.ForEach(obj => obj->box.center -= glm::UP * (overlap * massUp / totalMass));
                        //upObjects.ForEach(obj => obj->box.center += glm::UP * (overlap * massDown / totalMass));
                        downBox.center -= glm::UP * (overlap + COLLISION_TOLERANCE);
                        Velocity& upVelocity = registry.get<Velocity>(upObject);
                        Velocity& downVelocity = registry.get<Velocity>(downObject);
                        upVelocity.velocity.y = downVelocity.velocity.y;
                        downVelocity.velocity.y = 0;
                    }

                    downAdjacencies.adjacencies[Direction::UP] = upObject;
                    upAdjacencies.adjacencies[Direction::DOWN] = downObject;
}


void collideDynamicWithDynamicEntityVertically(entt::registry& registry, entt::entity e1, entt::entity e2)
  {
    auto& box1 = registry.get<Box>(e1);
    auto& box2 = registry.get<Box>(e2);
    entt::entity downObject, upObject;
    if (box1.top() - box2.bottom() < box2.top() - box1.bottom())
    {
      downObject = e1;
      upObject = e2;
    }
    else
    {
      downObject = e2;
      upObject = e1;
    }

    collideDownWithUpDynamicEntityVertically(registry, downObject, upObject);
}

bool collideDynamicEntitiesVertically(entt::registry& registry)
{
  bool collisionDetected = false;
  auto dynamicEntities = registry.view<Box, const Velocity, const Mass, Adjacencies>();
  auto collidableEntities = registry.view<Box>();
  dynamicEntities.each([&](const entt::entity e1, Box& box1, const Velocity& velocity, const Mass mass, Adjacencies& adjacencies) 
  {
      collidableEntities.each([&](const entt::entity e2, Box& box2)
      {
        if (e1 != e2 && box1.intersects(box2))
        {
          collisionDetected = true;
          if(registry.all_of<Box, Velocity, Mass, Adjacencies>(e2))
          {
            collideDynamicWithDynamicEntityVertically(registry, e1, e2);
          }
          else
          {
            collideDynamicWithStaticEntityVertically(registry, box1, adjacencies, box2);
          }
        }       
      });
  });
  return collisionDetected;
}

void collideDynamicWithStaticEntityHorizontally(entt::registry& registry, Box& box1, Adjacencies& adjacencies, Box& box2)
{
  float dynamicStaticOverlap = box1.right() - box2.left();
          float staticDynamicOverlap = box2.right() - box1.left();
          if (dynamicStaticOverlap < staticDynamicOverlap)
          {
              adjacencies.is_on_wall_right = true;
              box1.center -= glm::RIGHT * (dynamicStaticOverlap + COLLISION_TOLERANCE);
              std::vector<entt::entity> leftObjects = adjacencies.get_adjacency_list(registry, Direction::LEFT);
              for (entt::entity obj : leftObjects)
              {
                Adjacencies& objAjacencies = registry.get<Adjacencies>(obj);
                objAjacencies.is_on_wall_right = true;
                Box& objBox = registry.get<Box>(obj);
                objBox.center -= glm::RIGHT * dynamicStaticOverlap;
              }
          }
          else
          {
            adjacencies.is_on_wall_left = true;
            box1.center += glm::RIGHT * (staticDynamicOverlap + COLLISION_TOLERANCE);
            std::vector<entt::entity> rightObjects = adjacencies.get_adjacency_list(registry, Direction::RIGHT);
            for (entt::entity obj : rightObjects)
            {
              Adjacencies& objAjacencies = registry.get<Adjacencies>(obj);
              objAjacencies.is_on_wall_left = true;
              Box& objBox = registry.get<Box>(obj);
              objBox.center += glm::RIGHT * staticDynamicOverlap;
            }
          }
  }

  void collideLeftWithRightDynamicEntityHorizontally(entt::registry& registry, entt::entity leftEntity, entt::entity rightEntity)
  {
    auto [leftBox, leftAdjacencies, leftMass] = registry.get<Box, Adjacencies, const Mass>(leftEntity);
    auto [rightBox, rightAdjacencies, rightMass] = registry.get<Box, Adjacencies, const Mass>(rightEntity);

    float overlap = leftBox.right() - rightBox.left();

        std::vector<entt::entity> leftObjects = leftAdjacencies.get_adjacency_list(registry, Direction::LEFT);
        std::vector<entt::entity> rightObjects = rightAdjacencies.get_adjacency_list(registry, Direction::RIGHT);

        float massLeft = std::accumulate(leftObjects.cbegin(), leftObjects.cend(), leftMass.mass, 
          [&](float acc, entt::entity e){return acc + registry.get<const Mass>(e).mass;});
        float massRight = std::accumulate(rightObjects.cbegin(), rightObjects.cend(), rightMass.mass, 
          [&](float acc, entt::entity e){return acc + registry.get<const Mass>(e).mass;});
        float totalMass = massLeft + massRight;

        leftBox.center -= glm::RIGHT * (overlap * massRight / totalMass + COLLISION_TOLERANCE);
        rightBox.center += glm::RIGHT * (overlap * massLeft / totalMass + COLLISION_TOLERANCE);

        std::for_each(leftObjects.cbegin(), leftObjects.cend(), [&](entt::entity e){registry.get<Box>(e).center -= glm::RIGHT * (overlap * massRight / totalMass);});
        std::for_each(rightObjects.cbegin(), rightObjects.cend(), [&](entt::entity e){registry.get<Box>(e).center += glm::RIGHT * (overlap * massLeft / totalMass);});

        leftAdjacencies.adjacencies[Direction::RIGHT] = rightEntity;
        rightAdjacencies.adjacencies[Direction::LEFT] = leftEntity;
  }

  void collideDynamicWithDynamicEntityHorizontally(entt::registry& registry, entt::entity e1, entt::entity e2)
  {
    auto& box1 = registry.get<Box>(e1);
    auto& box2 = registry.get<Box>(e2);
    entt::entity leftObject, rightObject;
    if (box1.right() - box2.left() < box2.right() - box1.left())
    {
        leftObject = e1;
        rightObject = e2;
    }
    else
    {
        leftObject = e2;
        rightObject = e1;
    }

    collideLeftWithRightDynamicEntityHorizontally(registry, leftObject, rightObject);
}

bool collideDynamicEntitiesHorizontally(entt::registry& registry)
{
  bool collisionDetected = false;
  auto dynamicEntities = registry.view<Box, const Velocity, const Mass, Adjacencies>();
  auto collidableEntities = registry.view<Box>();
  dynamicEntities.each([&](const entt::entity e1, Box& box1, const Velocity& velocity, const Mass mass, Adjacencies& adjacencies) 
  {
    collidableEntities.each([&](const entt::entity e2, Box& box2)
    {
        if (e1 != e2 && box1.intersects(box2))
        {
          collisionDetected = true;
          if(registry.all_of<Box, Velocity, Mass, Adjacencies>(e2))
          {
            collideDynamicWithDynamicEntityHorizontally(registry, e1, e2);
          }
          else
          {
            collideDynamicWithStaticEntityHorizontally(registry, box1, adjacencies, box2);
          }
        }   
      });
  });
  return collisionDetected;
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

  const entt::entity floor = registry.create();
  registry.emplace<Box>(floor, Box(glm::vec2(WINDOW_WIDTH/2, 0), glm::vec2(WINDOW_WIDTH/2, 8)));
  registry.emplace<Sprite>(floor, textureManager.normalTexture);

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

    applyInputToVelocity(registry, keystate, gravity_enabled);

    resetAdjacencies(registry);

    const int maxNumCollisionIterations = 100;

    applyVerticalVelocityToPosition(registry);
    bool verticalCollisionDetected = true;
    int numVerticalCollisionIterations = 0;
    while(verticalCollisionDetected && numVerticalCollisionIterations < maxNumCollisionIterations)
    {
      verticalCollisionDetected = collideDynamicEntitiesVertically(registry);
      numVerticalCollisionIterations++;
    }

    // std::cout << "numVerticalCollisionIterations: " << numVerticalCollisionIterations << std::endl;
  
    applyHorizontalVelocityToPosition(registry);
    bool horizontalCollisionDetected = true;
    int numHorizontalCollisionIterations = 0;
    while(horizontalCollisionDetected && numHorizontalCollisionIterations < maxNumCollisionIterations)
    {
      horizontalCollisionDetected = collideDynamicEntitiesHorizontally(registry);
      numHorizontalCollisionIterations++;
    }

    // std::cout << "numHorizontalCollisionIterations: " << numHorizontalCollisionIterations << std::endl;

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
