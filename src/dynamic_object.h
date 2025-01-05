#include "game_object.h"
#include <unordered_map>
#include <vector>

enum direction { UP, DOWN, LEFT, RIGHT};

class dynamic_object : public game_object
{
    public:
    dynamic_object(aabb box, renderable* game_object_renderable);
    virtual ~dynamic_object() = default;
    std::vector<dynamic_object*> get_adjacency_list(direction dir);
    void reset();

    vec2 velocity;
    float mass;

    bool isOnCeiling;
	bool isOnFloor;
	bool isOnWallLeft;
	bool isOnWallRight;

    std::unordered_map<direction, dynamic_object*> adjacencies;
};