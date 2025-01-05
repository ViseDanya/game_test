
#include "aabb.h"

aabb::aabb(vec2 center, vec2 size) : center(center), size(size) {}

float aabb::left() const { return center.x - size.x; }
float aabb::right() const{ return center.x + size.x; }
float aabb::top() const{ return center.y + size.y; }
float aabb::bottom() const{ return center.y - size.y; }
vec2 aabb::top_left() const{ return vec2(center.x - size.x, size.y + size.y); }
vec2 aabb::top_right() const{ return center + size; }
vec2 aabb::bottom_left() const{ return center - size; }
vec2 aabb::bottom_right() const{ return vec2(center.x + size.x, center.y - size.y); }

bool aabb::intersects(const aabb& other) const
{
    return  right() > other.left() &&
            other.right() > left() &&
            top() > other.bottom() &&
            other.top() > bottom();
}