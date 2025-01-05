#include "dynamic_object.h"
#include "SDL3/SDL.h"

class player : public dynamic_object
{
    public:
    player(vec2 position);
    virtual ~player() = default;

    void process_input(const bool* keystate, const bool gravity_enabled);
    SDL_Scancode left_key;
    SDL_Scancode right_key;
    SDL_Scancode up_key;
    SDL_Scancode down_key;
};