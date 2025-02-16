
#include "Box.h"

Box::Box(glm::vec2 center, glm::vec2 size) : center(center), size(size) {}

float Box::left() const { return center.x - size.x; }
float Box::right() const{ return center.x + size.x; }
float Box::top() const{ return center.y + size.y; }
float Box::bottom() const{ return center.y - size.y; }
glm::vec2 Box::top_left() const{ return glm::vec2(center.x - size.x, size.y + size.y); }
glm::vec2 Box::top_right() const{ return center + size; }
glm::vec2 Box::bottom_left() const{ return center - size; }
glm::vec2 Box::bottom_right() const{ return glm::vec2(center.x + size.x, center.y - size.y); }

bool Box::intersects(const Box& other) const
{
    return  right() > other.left() &&
            other.right() > left() &&
            top() > other.bottom() &&
            other.top() > bottom();
}