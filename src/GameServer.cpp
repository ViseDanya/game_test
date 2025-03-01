#include "GameServer.h"
#include "Entity.h"
#include "Systems/RenderSystem.h"
#include "Systems/AnimationSystem.h"
#include "Systems/CollisionSystem.h"
#include "Systems/InputSystem.h"
#include "Systems/PhysicsSystem.h"
#include "Constants.h"
#include "TextureManager.h"
#include "Velocity.h"
#include "Adjacencies.h"
#include "Networking/Message.h"

extern SDL_Renderer* sdlRenderer;
static Camera camera;
static bool gravity_enabled = false;

void GameServer::run()
{
    startServer();

    TextureManager::loadAllTextures(sdlRenderer);
    Renderer renderer(sdlRenderer);

    bool quit = false;
  SDL_Event event;

  float mouseX;
  float mouseY;

  camera.position = glm::vec2(WINDOW_WIDTH/2, WINDOW_HEIGHT/2);
  camera.zoom = 1;

  bool manualCamera = true;
//   entt::registry registry;
//   createGameScene(registry);
//   resetCamera();  

  while (!quit)
  {
    Uint64 frameStartTime = SDL_GetTicks();
    while (SDL_PollEvent(&event) != 0)
    {
    //   ImGui_ImplSDL3_ProcessEvent(&event);
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

    resetVelocity(registry, false);//gravityEnabled);

    // applyInputToVelocity(registry, keystate, false);//gravityEnabled);
    processEvents();

    for (const auto& [clientID, playerInputMessage] : playerInputState) 
    {
      if(players.find(clientID) != players.end())
      {
        handlePlayerInput(players[clientID], playerInputMessage);
      }
    }

    resetAdjacencies(registry);
    applyVelocityToPosition(registry);
    resolveCollisions(registry);
    updateFakePlatforms(registry);

    broadcastUpdatesToClients();

    SDL_SetRenderDrawColor(sdlRenderer, 0, 0, 0, 255);
    SDL_RenderClear(sdlRenderer);

    updateAnimators(registry);
    updateTrampolineAnimations(registry);
    updateAnimations(registry);
    renderColoredEntities(registry, renderer, camera);
    renderSprites(registry, renderer, camera);

    // showImGui(registry);

    // if(debugColliders)
    // {
    //   renderDebugColliders(registry, renderer, camera);
    // }

    SDL_RenderPresent(sdlRenderer);

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

    for (const auto& pair : players) 
    {
      entt::entity player = pair.second;
      const Box& box = registry.get<Box>(player);
      game::Message createEntityMessage = createCreateEntityMessage(player, EntityType::PLAYER, box.center);
      sendMessageToClient(event.peer->incomingPeerID, createEntityMessage);
    }

    glm::vec2 position = glm::vec2(WINDOW_WIDTH/2, WINDOW_HEIGHT/2);
    entt::entity player = createPlayerEntity(registry, position);
    players[event.peer->incomingPeerID] = player;

    game::Message createEntityMessage = createCreateEntityMessage(player, EntityType::PLAYER, position);
    broadcastMessageToClients(createEntityMessage);

    std::cout << "handleClientConnected done" << std::endl;
}

void GameServer::handleClientDisconnected(const ENetEvent& event)
{
    
}

void GameServer::handleMessageReceived(const ENetEvent& event)
{
  std::cout << "GameServer::handleMessageReceived" << std::endl;
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
      std::cout << "Received playerInputMessage: " << event.peer->incomingPeerID << std::endl;
      playerInputState[event.peer->incomingPeerID] = playerInputMessage;
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
    if(!gravity_enabled)
    {
      velocity.velocity.y += PLAYER_SPEED * 1.f/FPS;
    }
    else if(adjacencies.isOnFloor)
    {
      velocity.velocity.y += PLAYER_JUMP_SPEED * 1.f/FPS;
    }
  }
  if (playerInputMessage.down() && !gravity_enabled)
  {
      velocity.velocity.y -= PLAYER_SPEED * 1.f/FPS;
  }
}

void GameServer::broadcastUpdatesToClients()
{
  for (const auto& pair : players) 
  {
    entt::entity player = pair.second;
    game::Message playerUpdateMessage = createDynamicEntityUpdateMessage(registry, player);
    broadcastMessageToClients(playerUpdateMessage);
  }

  game::Message cameraUpdateMessage = createCameraUpdateMessage(camera);
  broadcastMessageToClients(cameraUpdateMessage);
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