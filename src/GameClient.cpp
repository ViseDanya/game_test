#include "GameClient.h"
#include "Entity.h"
#include "Systems/RenderSystem.h"
#include "Systems/AnimationSystem.h"
#include "Systems/CollisionSystem.h"
#include "Systems/InputSystem.h"
#include "Systems/PhysicsSystem.h"
#include "Constants.h"
#include "TextureManager.h"
#include "game.pb.h"

extern SDL_Renderer* sdlRenderer;
static Camera camera;

GameClient::GameClient()
{
    client.handleServerDisconnected = [&](ENetEvent event){handleServerDisconnected(event);};
    client.handleMessageReceived = [&](ENetEvent event){handleMessageReceived(event);};
}

void GameClient::processAndSendInput(const bool* keystate)
{
    game::PlayerInputMessage playerInputMessage;
    playerInputMessage.set_entity(entt::to_integral(playerEntity));
    if (keystate[SDL_SCANCODE_A])
    {
        playerInputMessage.set_left(true);
    }
    if (keystate[SDL_SCANCODE_D])
    {
        playerInputMessage.set_right(true);
    }
    if (keystate[SDL_SCANCODE_W])
    {
        playerInputMessage.set_up(true);
    }
    if (keystate[SDL_SCANCODE_S])
    {
        playerInputMessage.set_down(true);
    }

    game::Message message;
    message.set_message_type(game::MessageType::PLAYER_INPUT_MESSAGE);
    message.mutable_player_input_message()->CopyFrom(playerInputMessage);
    std::string serializedMessage;
    message.SerializeToString(&serializedMessage);
    client.sendMessageToServer(serializedMessage.c_str(), serializedMessage.length());
}

void GameClient::run()
{
    client.connectToServer("localhost");

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

    const bool *keystate = SDL_GetKeyboardState(nullptr);
    processAndSendInput(keystate);

    client.processEvents();

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

void GameClient::handleServerDisconnected(ENetEvent event)
{
    
}

void GameClient::handleMessageReceived(ENetEvent event)
{
  std::cout << "GameClient::handleMessageReceived" << std::endl;
  game::Message message;
  if(!message.ParseFromArray(event.packet->data, event.packet->dataLength))
  {
    std::cerr << "Failed to parse messsage." << std::endl;
  };

  // Check the message type and process accordingly
  switch (message.message_type()) 
  {
    case game::CREATE_ENTITY_MESSAGE:
    {
      // Process ExampleMessage
      const game::CreateEntityMessage& createEntityMessage = message.create_entity_message();
      std::cout << "Received createEntityMessage: " << createEntityMessage.entity_type() << std::endl;
      glm::vec2 position;
      position.x = createEntityMessage.position().x();
      position.y = createEntityMessage.position().y();
      entt::entity clientEntity = createEntity(static_cast<EntityType>(createEntityMessage.entity_type()), registry, position);
      serverToClientEntityMap[entt::entity{createEntityMessage.entity()}] = clientEntity;
      break;
    }
    default:
    {
      std::cerr << "Unknown message type!" << std::endl;
      break;
    }
  }
}