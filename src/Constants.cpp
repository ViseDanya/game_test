#include "Constants.h"

std::ostream& operator<<(std::ostream& stream, const glm::vec2& v)
{
    stream << v.x << v.y;
    return stream;
}