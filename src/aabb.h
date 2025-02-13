#pragma once

#include "vec2.h"

class aabb
{
    public:
        aabb() = default;
        aabb(vec2 center, vec2 size);
        float left() const;
        float right() const;
        float top() const;
        float bottom() const;
        vec2 top_left() const;
        vec2 top_right() const;
        vec2 bottom_left() const;
        vec2 bottom_right() const;;

        bool intersects(const aabb& other) const;

        vec2 center;
        vec2 size;
};