#include "InputController.h"
#include "Player.h"

const float PLAYER_SPEED = 180;
const float PLAYER_JUMP_HEIGHT = 60;
const float PLAYER_JUMP_TIME_TO_APEX = .25;
const float GRAVITY = -2*PLAYER_JUMP_HEIGHT/(PLAYER_JUMP_TIME_TO_APEX*PLAYER_JUMP_TIME_TO_APEX);
const float PLAYER_JUMP_SPEED = (PLAYER_JUMP_HEIGHT - .5 * GRAVITY * PLAYER_JUMP_TIME_TO_APEX * PLAYER_JUMP_TIME_TO_APEX)/(PLAYER_JUMP_TIME_TO_APEX);

class PlayerInputController : public InputController
{
    public:
    PlayerInputController(Player& p) : p(p) {};
    virtual void ProcessInput(const bool* keystate) override;
    SDL_Scancode left_key;
    SDL_Scancode right_key;
    SDL_Scancode up_key;
    SDL_Scancode down_key;
    bool gravity_enabled;

    private:
    Player& p;
};