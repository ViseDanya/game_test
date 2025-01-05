#include "game_object.h"
#include "renderable.h"
#include "constants.h"

game_object::game_object(aabb box, renderable* game_object_renderable): box(box), game_object_renderable(game_object_renderable) {};
game_object::~game_object()
{ 
    if(game_object_renderable)
    {
        delete game_object_renderable; 
    }
};
void game_object::render()
{
    game_object_renderable->render(box);
}