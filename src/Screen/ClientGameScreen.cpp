#include "ClientGameScreen.h"

#include "ClientGameScreen.h"
#include "Entity.h"
#include "Systems/RenderSystem.h"
#include "Systems/AnimationSystem.h"
#include "Systems/CollisionSystem.h"
#include "Systems/InputSystem.h"
#include "Systems/PhysicsSystem.h"
#include "Constants.h"
#include "SDLTextureManager.h"
#include "Components/Box.h"
#include "Components/Velocity.h"
#include "Components/Adjacencies.h"
#include "Components/Animation.h"
#include "Components/Health.h"
#include "Components/Name.h"
#include <game.pb.h>
#include <imgui.h>

ClientGameScreen::ClientGameScreen(GameClient* client)
    : client(client)
{
}

ClientGameScreen::~ClientGameScreen()
{
    delete client;
}

void ClientGameScreen::render(SDL_Renderer* renderer)
{
    ImGui::SetNextWindowPos({10, 10});
    ImGui::Begin("Options", nullptr, ImGuiWindowFlags_AlwaysAutoResize);
    if (ImGui::Button("Ready"))
    {
      client->ready = !client->ready;
      client->sendReady();
    }
    ImGui::SameLine();
    ImGui::Text(client->ready ? "Ready" : "Not Ready");

    auto view = client->registry.view<const Name, const Health>();
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

    updateAnimators(client->registry);
    updateTrampolineAnimations(client->registry);
    updateAnimations(client->registry);
    renderColoredEntities(client->registry, renderer, client->camera);
    renderSprites(client->registry, renderer, client->camera);
}

void ClientGameScreen::sendInput()
{
    const bool *keystate = SDL_GetKeyboardState(nullptr);

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
    client->sendUnreliableMessage(serializedMessage.c_str(), serializedMessage.length());
}

void ClientGameScreen::update(const float dt)
{
    sendInput();
    client->processEvents();
}


