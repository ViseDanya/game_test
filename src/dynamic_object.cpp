#include "dynamic_object.h"
#include <memory>

dynamic_object::dynamic_object(aabb box, renderable* game_object_renderable) : game_object(box, game_object_renderable)
{
    reset();
}

void dynamic_object::reset()
{
    isOnCeiling = false;
    isOnFloor = false;
    isOnWallLeft = false;
    isOnWallRight = false;
    adjacencies.clear();
}

std::vector<dynamic_object*> dynamic_object::get_adjacency_list(direction dir)
{
    std::vector<dynamic_object*> adjacency_list;
	dynamic_object* current_obj = this;
    while(current_obj->adjacencies.find(dir) != adjacencies.end())
    {
        current_obj = current_obj->adjacencies[dir];
        adjacency_list.push_back(current_obj);
    }
    return adjacency_list;
}