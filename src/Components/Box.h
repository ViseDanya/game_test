#pragma once

#include <glm/vec2.hpp>

class Box
{
    public:
        Box() = default;
        Box(glm::vec2 center, glm::vec2 size) : center(center), size(size) {}
        float left() const { return center.x - size.x; }
        float right() const{ return center.x + size.x; }
        float top() const{ return center.y + size.y; }
        float bottom() const{ return center.y - size.y; }
        glm::vec2 topLeft() const{ return glm::vec2(center.x - size.x, center.y + size.y); }
        glm::vec2 topRight() const{ return center + size; }
        glm::vec2 bottomLeft() const{ return center - size; }
        glm::vec2 bottomRight() const{ return glm::vec2(center.x + size.x, center.y - size.y); }

        bool intersects(const Box& other) const
        {
            return  right() > other.left() &&
                    other.right() > left() &&
                    top() > other.bottom() &&
                    other.top() > bottom();
        }

        glm::vec2 center;
        glm::vec2 size;
};