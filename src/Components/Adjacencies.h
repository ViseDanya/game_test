#pragma once

#include <Direction.h>
#include <unordered_map>
#include <entt/entt.hpp>

struct Adjacencies
{
    bool isOnCeiling;
    bool isOnFloor;
    bool isOnWallLeft;
    bool isOnWallRight;
    std::unordered_map<Direction, entt::entity> adjacencies;

    void reset()
    {
      isOnCeiling = false;
      isOnFloor = false;
      isOnWallLeft = false;
      isOnWallRight = false;
      adjacencies.clear();  
    }

    bool getIsOnStaticObject(const Direction direction)
    {
      switch(direction)
      {
          case(Direction::UP):
              return isOnCeiling;
          case(Direction::DOWN):
              return isOnFloor;
          case(Direction::LEFT):
              return isOnWallLeft;
          case(Direction::RIGHT):
              return isOnWallRight;
      }
    }

    void setIsOnStaticObject(const Direction direction, const bool b)
    {
      switch(direction)
      {
          case(Direction::UP):
              isOnCeiling = b;
              break;
          case(Direction::DOWN):
              isOnFloor = b;
              break;
          case(Direction::LEFT):
              isOnWallLeft = b;
              break;
          case(Direction::RIGHT):
              isOnWallRight = b;
              break;
      }
    }
    
    std::vector<entt::entity> getAdjacencyList(entt::registry& registry, Direction dir)
    {
        std::vector<entt::entity> adjacency_list;
        std::unordered_map<Direction, entt::entity> currentAdjacencies = adjacencies;
        while(currentAdjacencies.find(dir) != currentAdjacencies.end())
        {
            entt::entity currentEntity = currentAdjacencies[dir];
            if(!registry.all_of<Adjacencies>(currentEntity))
            {
                break;
            }
            else
            {
                currentAdjacencies = registry.get<Adjacencies>(currentEntity).adjacencies;
                adjacency_list.push_back(currentEntity);
            }
        }
        return adjacency_list;
    }
};