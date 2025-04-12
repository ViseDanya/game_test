#include "GameClient.h"
#include "Entity.h"
#include "Systems/RenderSystem.h"
#include "Systems/AnimationSystem.h"
#include "Systems/CollisionSystem.h"
#include "Systems/InputSystem.h"
#include "Systems/PhysicsSystem.h"
#include "Constants.h"
#include "TextureManager.h"
#include "Components/Box.h"
#include "Components/Velocity.h"
#include "Components/Adjacencies.h"
#include "Components/Animation.h"
#include "Components/Health.h"
#include "Components/Name.h"
#include <proto/game.pb.h>
#include <imgui.h>
#include <imgui_impl_sdl3.h>
#include <imgui_impl_sdlrenderer3.h>

extern SDL_Renderer* sdlRenderer;
static Camera camera;
static char serverAddress[256] = "localhost";
static char name[256] = "Sexy Beast";
static bool connected = false;

void GameClient::sendReady()
{
  game::ClientReadyMessage clientReadyMessage;
  clientReadyMessage.set_ready(ready);
  game::Message message;
  message.set_message_type(game::MessageType::CLIENT_READY_MESSAGE);
  message.mutable_client_ready_message()->CopyFrom(clientReadyMessage);
  std::string serializedMessage;
  message.SerializeToString(&serializedMessage);
  sendMessageToServer(serializedMessage.c_str(), serializedMessage.length());
}

void GameClient::showUI()
{
    ImGui_ImplSDLRenderer3_NewFrame();
    ImGui_ImplSDL3_NewFrame();
    ImGui::NewFrame();
    ImGui::SetNextWindowPos({10, 10});
    ImGui::Begin("Options", nullptr, ImGuiWindowFlags_AlwaysAutoResize);
    if (ImGui::Button("Ready"))
    {
      ready = !ready;
      sendReady();
    }
    ImGui::SameLine();
    ImGui::Text(ready ? "Ready" : "Not Ready");

    auto view = registry.view<const Name, const Health>();
    int i = 0;
    view.each([&](const Name& name, const Health& health)
    {
      ImGui::PushID(i);
      std::stringstream ss;
      ss << name.name << ": " << health.health;
      ImGui::Text("%s", ss.str().c_str());
      ImGui::PopID();
    } );


    ImGui::End();

    ImGui::Render();
    ImGui_ImplSDLRenderer3_RenderDrawData(ImGui::GetDrawData(), sdlRenderer);
}

void GameClient::runConnectionLoop()
{
  bool quit = false;
  SDL_Event event;
  while (!quit && !connected)
  {
    while (SDL_PollEvent(&event) != 0)
    {
      ImGui_ImplSDL3_ProcessEvent(&event);
      if (event.type == SDL_EVENT_QUIT)
      {
        quit = true;
      }
    }

    showConnectUI();

    SDL_RenderPresent(sdlRenderer);
  }

  if(connected)
  {
    runGameLoop();
  }
}

void GameClient::showConnectUI()
{
    ImGui_ImplSDLRenderer3_NewFrame();
    ImGui_ImplSDL3_NewFrame();
    ImGui::NewFrame();
    ImGui::SetNextWindowPos({10, 10});
    ImGui::Begin("Options", nullptr, ImGuiWindowFlags_AlwaysAutoResize);

    ImGui::Text("Server Address");
    ImGui::SameLine();
    ImGui::InputText("##labelServer Address", serverAddress, IM_ARRAYSIZE(serverAddress));

    ImGui::Text("Name");
    ImGui::SameLine();
    ImGui::InputText("##labelName", name, IM_ARRAYSIZE(name));

    if (ImGui::Button("Connect"))
    {
      connectToServer(serverAddress);
    }

    ImGui::End();

    ImGui::Render();
    ImGui_ImplSDLRenderer3_RenderDrawData(ImGui::GetDrawData(), sdlRenderer);
}

void GameClient::processAndSendInput(const bool* keystate)
{
    game::PlayerInputMessage playerInputMessage;
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
    sendMessageToServer(serializedMessage.c_str(), serializedMessage.length());
}

void GameClient::run()
{
    // connectToServer("34.56.4.111");
    // connectToServer("localhost");
  runConnectionLoop();
}

void GameClient::runGameLoop()
{
  TextureManager::loadAllTextures(sdlRenderer);
    Renderer renderer(sdlRenderer);

    bool quit = false;
  SDL_Event event;

  float mouseX;
  float mouseY;
  camera.position = glm::vec2(WINDOW_WIDTH/2, WINDOW_HEIGHT/2);
  camera.zoom = 1;

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
    processAndSendInput(keystate);

    processEvents();

    SDL_SetRenderDrawColor(sdlRenderer, 0, 0, 0, 255);
    SDL_RenderClear(sdlRenderer);

    updateAnimators(registry);
    updateTrampolineAnimations(registry);
    updateAnimations(registry);
    renderColoredEntities(registry, renderer, camera);
    renderSprites(registry, renderer, camera);

    showUI();

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

void GameClient::handleServerConnected(const ENetEvent& event)
{
    connected = true;
    game::ClientNameMessage clientNameMessage;
    clientNameMessage.set_name(name);
    game::Message message;
    message.set_message_type(game::MessageType::CLIENT_NAME_MESSAGE);
    message.mutable_client_name_message()->CopyFrom(clientNameMessage);
    std::string serializedMessage;
    message.SerializeToString(&serializedMessage);
    sendMessageToServer(serializedMessage.c_str(), serializedMessage.length());
}

void GameClient::handleServerDisconnected(const ENetEvent& event)
{
    
}

void GameClient::handleMessageReceived(const ENetEvent& event)
{
  // std::cout << "GameClient::handleMessageReceived" << std::endl;
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
      std::cout << entt::to_integral(clientEntity) << std::endl;
      break;
    }
    case game::DYNAMIC_ENTITY_UPDATE_MESSAGE:
    {
      const game::DynamicEntityUpdateMessage& dynamicEntityUpdateMessage = message.dynamic_entity_update_messsage();
      // std::cout << "Received dynamicEntityUpdateMessage: " << dynamicEntityUpdateMessage.entity() << std::endl;
      const entt::entity serverEntity = entt::entity{dynamicEntityUpdateMessage.entity()};
      if(serverToClientEntityMap.find(serverEntity) != serverToClientEntityMap.end())
      {
        entt::entity clientEntity = serverToClientEntityMap[serverEntity];
        glm::vec2 position;
        position.x = dynamicEntityUpdateMessage.position().x();
        position.y = dynamicEntityUpdateMessage.position().y();
        Box& box = registry.get<Box>(clientEntity);
        box.center = position;
        glm::vec2 velocity;
        velocity.x = dynamicEntityUpdateMessage.velocity().x();
        velocity.y = dynamicEntityUpdateMessage.velocity().y();
        Velocity& velocityComponent = registry.get<Velocity>(clientEntity);
        velocityComponent.velocity = velocity;
        Adjacencies& adjacencies = registry.get<Adjacencies>(clientEntity);
        adjacencies.isOnFloor = dynamicEntityUpdateMessage.is_on_floor();
      }
      break;
    }
    case game::CAMERA_UPDATE_MESSAGE:
    {
      const game::CameraUpdateMessage& cameraUpdateMessage = message.camera_update_message();
      // std::cout << "Received cameraUpdateMessage: " << std::endl;
      camera.position.x = cameraUpdateMessage.position().x();
      camera.position.y = cameraUpdateMessage.position().y(); 
      camera.zoom = cameraUpdateMessage.zoom();
      break;
    }
    case game::PLAY_ANIMATION_MESSAGE:
    {
      const game::PlayAnimationMessage& playAnimationMessage = message.play_animation_message();
      const entt::entity serverEntity = entt::entity{playAnimationMessage.entity()};
      entt::entity clientEntity = serverToClientEntityMap[serverEntity];
      Animation& animation = registry.get<Animation>(clientEntity);
      animation.currentFrame = 0;
      animation.isPlaying = true;
      break;
    }
    case game::HEALTH_UPDATE_MESSAGE:
    {
      const game::HealthUpdateMessage& healthUpdateMessage = message.health_update_message();
      const entt::entity serverEntity = entt::entity{healthUpdateMessage.entity()};
      entt::entity clientEntity = serverToClientEntityMap[serverEntity];
      Health& health = registry.get<Health>(clientEntity);
      health.health = healthUpdateMessage.health();
      health.state = healthUpdateMessage.is_damaged() ? Health::State::DAMAGED : Health::State::IDLE;
      break;
    }
    case game::POSITION_UPDATE_MESSAGE:
    {
      const game::PositionUpdateMessage& positionUpdateMessage = message.position_update_message();
      const entt::entity serverEntity = entt::entity{positionUpdateMessage.entity()};
      entt::entity clientEntity = serverToClientEntityMap[serverEntity];
      Box& box = registry.get<Box>(clientEntity);
      box.center.x = positionUpdateMessage.position().x();
      box.center.y = positionUpdateMessage.position().y(); 
      break;
    }
    case game::DESTROY_ENTITY_MESSAGE:
    {
      const game::DestroyEntityMessage& destroyEntityMessage = message.destroy_entity_message();
      const entt::entity serverEntity = entt::entity{destroyEntityMessage.entity()};
      entt::entity clientEntity = serverToClientEntityMap[serverEntity];
      registry.destroy(clientEntity);
      break;
    }
    case game::NAME_UPDATE_MESSAGE:
    {
      const game::NameUpdateMessage& nameUpdateMessage = message.name_update_message();
      std::cout << "Received nameUpdateMessage: " << event.peer->incomingPeerID << ", "
      << nameUpdateMessage.name() << std::endl;

      const entt::entity serverEntity = entt::entity{nameUpdateMessage.entity()};
      entt::entity clientEntity = serverToClientEntityMap[serverEntity];
      std::cout << entt::to_integral(clientEntity) << std::endl;
      if(Name* name = registry.try_get<Name>(clientEntity))
      {
        name->name = nameUpdateMessage.name();
      }
      else
      {
        registry.emplace<Name>(clientEntity, nameUpdateMessage.name());
      }
      break;
    }
    default:
    {
      std::cerr << "Unknown message type!: " << message.message_type() << std::endl;
      break;
    }
  }
}