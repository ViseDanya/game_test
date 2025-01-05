#include "dynamic_object.h"

class player : public dynamic_object
{
    public:
    player(vec2 position);
    virtual ~player() = default;
};