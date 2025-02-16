#pragma once

#include <glm/vec2.hpp>

class Box
{
    public:
        Box() = default;
        Box(glm::vec2 center, glm::vec2 size);
        float left() const;
        float right() const;
        float top() const;
        float bottom() const;
        glm::vec2 top_left() const;
        glm::vec2 top_right() const;
        glm::vec2 bottom_left() const;
        glm::vec2 bottom_right() const;;

        bool intersects(const Box& other) const;

        glm::vec2 center;
        glm::vec2 size;
};