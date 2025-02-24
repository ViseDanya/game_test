#include "Direction.h"
#include "Constants.h"

Direction getOppositeDirection(const Direction d)
{
    switch(d)
    {
        case(Direction::UP):
            return Direction::DOWN;
        case(Direction::DOWN):
            return Direction::UP;
        case(Direction::LEFT):
            return Direction::RIGHT;
        case(Direction::RIGHT):
            return Direction::LEFT;
    }
}

glm::vec2 getDirectionAsVector(const Direction d)
{
    switch(d)
    {
        case(Direction::UP):
            return glm::UP;
        case(Direction::DOWN):
            return glm::DOWN;
        case(Direction::LEFT):
            return glm::LEFT;
        case(Direction::RIGHT):
            return glm::RIGHT;
    }
}