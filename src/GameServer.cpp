#include "GameServer.h"
#include "Entity.h"
#include "Systems/RenderSystem.h"
#include "Systems/AnimationSystem.h"
#include "Systems/CollisionSystem.h"
#include "Systems/InputSystem.h"
#include "Systems/PhysicsSystem.h"
#include "Systems/StateSystem.h"
#include "Components/Trampoline.h"
#include "Components/Fake.h"
#include "Constants.h"
#include "TextureManager.h"
#include "Components/Velocity.h"
#include "Components/Adjacencies.h"
#include "Components/Health.h"
#include "Components/Ceiling.h"
#include "Networking/Message.h"
#include "Scene.h"
#include <imgui.h>
#include <imgui_impl_sdl3.h>
#include <imgui_impl_sdlrenderer3.h>

extern SDL_Renderer* sdlRenderer;
static Camera camera;
static bool gravityEnabled = false;
static bool manualCamera = true;
static bool debugColliders = true;
static bool gameStarted = false;

void GameServer::createGameScene()
{
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

glm::vec2 GameServer::getCeilingPosition()
{
  return glm::vec2(camera.position.x, camera.position.y + WINDOW_HEIGHT/2 - PLATFORM_HEIGHT/2);
}

void GameServer::updateCeiling()
{
  if(registry.valid(ceiling))
  {
    Box& box = registry.get<Box>(ceiling);
    box.center.y -= CAMERA_SPEED;
  }
}

void GameServer::spawnPlatform()
{
  createEvenlySpacedPlatforms(registry, platformSpawnPoint, 1);
  platformSpawnPoint -= 3*PLATFORM_HEIGHT;
}

void GameServer::spawnWalls()
{
  createWallEntity(registry, glm::vec2(WALL_WIDTH/2, wallSpawnPoint));
  createWallEntity(registry, glm::vec2(WINDOW_WIDTH - WALL_WIDTH/2, wallSpawnPoint));
  wallSpawnPoint -= WALL_HEIGHT;
}

void GameServer::resetCamera()
{
  camera.position = glm::vec2(WINDOW_WIDTH/2, WINDOW_HEIGHT/2);
  camera.zoom = 1;
}

void GameServer::showImGui()
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
    if(ImGui::Button("Start Game"))
    {
      createGameScene();
      gameStarted = true;
    }
    ImGui::End();

    ImGui::Render();
    ImGui_ImplSDLRenderer3_RenderDrawData(ImGui::GetDrawData(), sdlRenderer);
}

void GameServer::onEntityCreated(entt::entity entity)
{
  EntityType type = registry.get<TypeComponent>(entity).type;
  Box& box = registry.get<Box>(entity);
  game::Message createEntityMessage = createCreateEntityMessage(entity, type, box.center);
  broadcastMessageToClients(createEntityMessage);
}

void GameServer::onEntityDestroyed(entt::entity entity)
{
  std::cout << "onEntityDesstroyed" << std::endl;
  game::Message destroyEntityMessage = createDestroyEntityMessage(entity);
  broadcastMessageToClients(destroyEntityMessage);
  std::cout << "entitydddestroyed sent" << std::endl;
}

void GameServer::updatePlayersAlive()
{
  for (const auto& [clientID, client] : gameClients) 
  {
    if(registry.valid(client.player))
    {
      const Health& health = registry.get<Health>(client.player);
      const Box& box = registry.get<Box>(client.player);
      if(health.health == 0 || (box.bottom() < camera.position.y - WINDOW_HEIGHT/2 - 3*PLATFORM_HEIGHT))
      {
        registry.destroy(client.player);
      }
    }
  }
}

bool GameServer::areAllPlayersDead()
{
  for (const auto& [clientID, client] : gameClients) 
  {
    if(registry.valid(client.player))
    {
      return false;
    }
  }
  return true;
}

void GameServer::run()
{
    startServer();
    registry.on_construct<TypeComponent>().connect<&GameServer::onEntityCreated>(*this);
    registry.on_destroy<entt::entity>().connect<&GameServer::onEntityDestroyed>(*this);

    TextureManager::loadAllTextures(sdlRenderer);
    Renderer renderer(sdlRenderer);

    bool quit = false;
  SDL_Event event;

  float mouseX;
  float mouseY;

  resetCamera();

  while (!quit)
  {
    Uint64 frameStartTime = SDL_GetTicks();
    while (SDL_PollEvent(&event) != 0)
    {
      #ifndef HEADLESS
      ImGui_ImplSDL3_ProcessEvent(&event);
      #endif
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

    if(gameStarted && camera.position.y - platformSpawnPoint < 2*WINDOW_HEIGHT)
    {
      spawnPlatform();
    }
    if(gameStarted && camera.position.y - wallSpawnPoint < 2*WINDOW_HEIGHT)
    {
      spawnWalls();
    }
    
    resetVelocity(registry, gravityEnabled);

    processEvents();

    for (const auto& [clientID, client] : gameClients) 
    {
      if(registry.valid(client.player))
      {
        handlePlayerInput(client.player, client.playerInputState);
      }
    }

    resetAdjacencies(registry);
    applyVelocityToPosition(registry);
    updateTrampolines(registry);
    updateFakePlatforms(registry);
    updateHealth(registry);
    resolveCollisions(registry);
    updatePlayersAlive();

    broadcastGameUpdates();

    if(areAllPlayersDead())
    {
      registry.clear();
      resetCamera();
      gameStarted = false;
      gravityEnabled = false;
      manualCamera = true;
      for (auto& [clientID, client] : gameClients)
      {
        client.ready = false;
        entt::entity player = createPlayerEntity(registry, glm::vec2(WINDOW_WIDTH/2, WINDOW_HEIGHT/2));
        client.player = player;
      }
    }

    #ifndef HEADLESS
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
    #endif
    Uint64 frameEndTime = SDL_GetTicks();
    Uint64 elapsedTime = frameEndTime - frameStartTime;
    if(elapsedTime < 1000/FPS)
    {
      SDL_Delay(1000/FPS - elapsedTime);
    }
  }
}

void GameServer::handleClientConnected(const ENetEvent& event)
{
    std::cout << "handleClientConnected" << std::endl;

    for (const auto& pair : gameClients) 
    {
      entt::entity player = pair.second.player;
      const Box& box = registry.get<Box>(player);
      game::Message createEntityMessage = createCreateEntityMessage(player, EntityType::PLAYER, box.center);
      sendMessageToClient(event.peer->incomingPeerID, createEntityMessage);
    }

    glm::vec2 position = glm::vec2(WINDOW_WIDTH/2, WINDOW_HEIGHT/2);
    entt::entity player = createPlayerEntity(registry, position);
    gameClients[event.peer->incomingPeerID].player = player;

    // game::Message createEntityMessage = createCreateEntityMessage(player, EntityType::PLAYER, position);
    // broadcastMessageToClients(createEntityMessage);

    std::cout << "handleClientConnected done" << std::endl;
}

void GameServer::handleClientDisconnected(const ENetEvent& event)
{
    
}

void GameServer::handleMessageReceived(const ENetEvent& event)
{
  // std::cout << "GameServer::handleMessageReceived" << std::endl;
  game::Message message;
  if(!message.ParseFromArray(event.packet->data, event.packet->dataLength))
  {
    std::cerr << "Failed to parse messsage." << std::endl;
  };

  // Check the message type and process accordingly
  switch (message.message_type()) 
  {
    case game::PLAYER_INPUT_MESSAGE: 
    {
      // Process ExampleMessage
      const game::PlayerInputMessage& playerInputMessage = message.player_input_message();
      // std::cout << "Received playerInputMessage: " << event.peer->incomingPeerID << std::endl;
      gameClients[event.peer->incomingPeerID].playerInputState = playerInputMessage;
      break;
    }
    case game::CLIENT_READY_MESSAGE:
    {
      const game::ClientReadyMessage& clientReadyMesssage = message.client_ready_message();
      gameClients[event.peer->incomingPeerID].ready = clientReadyMesssage.ready();
      std::cout << "Received clientReadyMessage: " << event.peer->incomingPeerID << ", "
                << clientReadyMesssage.ready() << std::endl;
      if(areAllPlayersReady())
      {
        startGame();
      }
      break;
    }
    default:
    {
      std::cerr << "Unknown message type!" << std::endl;
      break;
    }
  }
}

void GameServer::handlePlayerInput(entt::entity player, const game::PlayerInputMessage& playerInputMessage)
{
  Velocity& velocity = registry.get<Velocity>(player);
  Adjacencies& adjacencies = registry.get<Adjacencies>(player);
  if (playerInputMessage.left())
  {
      velocity.velocity.x -= PLAYER_SPEED * 1.f/FPS;
  }
  if (playerInputMessage.right())
  {
      velocity.velocity.x += PLAYER_SPEED * 1.f/FPS;
  }
  if (playerInputMessage.up())
  {
    if(!gravityEnabled)
    {
      velocity.velocity.y += PLAYER_SPEED * 1.f/FPS;
    }
    else if(adjacencies.isOnFloor)
    {
      velocity.velocity.y += PLAYER_JUMP_SPEED * 1.f/FPS;
    }
  }
  if (playerInputMessage.down() && !gravityEnabled)
  {
      velocity.velocity.y -= PLAYER_SPEED * 1.f/FPS;
  }
}

void GameServer::broadcastGameUpdates()
{
  broadcastDynamicEntityUpdates();
  broadcastCameraUpdates();
  broadcastTrampolineUpdates();
  broadcastFakeUpdates();
  broadcastHealthUpdates();
  broadcastCeilingUpdates();
}

void GameServer::broadcastDynamicEntityUpdates()
{
  for (const auto& pair : gameClients) 
  {
    entt::entity player = pair.second.player;
    if(registry.valid(player))
    {
      game::Message playerUpdateMessage = createDynamicEntityUpdateMessage(registry, player);
      broadcastMessageToClients(playerUpdateMessage);
    }
  }
}

void GameServer::broadcastCameraUpdates()
{
  game::Message cameraUpdateMessage = createCameraUpdateMessage(camera);
  broadcastMessageToClients(cameraUpdateMessage);
}

void GameServer::broadcastTrampolineUpdates()
{
  auto view = registry.view<Trampoline>();
  view.each([&](entt::entity e, Trampoline& trampoline) 
  {
    if(trampoline.state == Trampoline::State::TRIGGERED)
    {
      game::Message playAnimationMessaage = createPlayAnimationMessage(e);
      broadcastMessageToClients(playAnimationMessaage);
    }
  });
}

void GameServer::broadcastFakeUpdates()
{
  auto view = registry.view<Fake>();
  view.each([&](entt::entity e, Fake& fake) 
  {
    if(fake.state == Fake::State::FLIP)
    {
      game::Message playAnimationMessaage = createPlayAnimationMessage(e);
      broadcastMessageToClients(playAnimationMessaage);
    }
  });
}

void GameServer::broadcastHealthUpdates()
{
  auto view = registry.view<Health>();
  view.each([&](entt::entity e, Health& health) 
  {
    game::Message healthUpdateMessage = createHealthUpdateMessage(e, health);
    broadcastMessageToClients(healthUpdateMessage);
  });
}

void GameServer::broadcastCeilingUpdates()
{
  auto view = registry.view<const Box, const Ceiling>();
  view.each([&](entt::entity e, const Box& box) 
  {
    game::Message positionUpdateMessage = createPositionUpdateMessage(e, box.center);
    broadcastMessageToClients(positionUpdateMessage);
  });
}

void GameServer::sendMessageToClient(enet_uint16 clientID, const game::Message& message)
{
  std::string serializedMessage;
  message.SerializeToString(&serializedMessage);
  ENetServer::sendMessageToClient(clientID, serializedMessage.c_str(), serializedMessage.length());
}

void GameServer::broadcastMessageToClients(const game::Message& message)
{
  std::string serializedMessage;
  message.SerializeToString(&serializedMessage);
  ENetServer::broadcastMessageToClients(serializedMessage.c_str(), serializedMessage.length());
}

bool GameServer::areAllPlayersReady()
{
  for (const auto & pair : gameClients)
  {
    if(!pair.second.ready)
    {
      return false;
    }
  }
  return true;
}

void GameServer::startGame()
{
  createGameScene();
  gameStarted = true;
  gravityEnabled = true;
  manualCamera = false;
}