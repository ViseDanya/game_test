#include "PhysicsSystem.h"
#include "Components/Box.h"
#include "Components/Velocity.h"
#include "Components/Adjacencies.h"
#include "Components/PositionHistory.h"
#include "Constants.h"
#include <entt/entt.hpp>

void updatePositionHistory(entt::registry& registry)
{
  auto view = registry.view<const Box, PositionHistory>();
  view.each([&](const Box& box, PositionHistory& positionHistory) 
  {
    positionHistory.prevPosition = box.center;
  });
}

void resetVelocity(entt::registry& registry, const bool gravityEnabled)
{
  auto view = registry.view<Velocity, const Adjacencies>();
  view.each([&](Velocity& velocity, const Adjacencies& adjacencies) 
  {
    velocity.velocity.x = 0;
    if(adjacencies.isOnFloor || !gravityEnabled)
    {
      velocity.velocity.y = 0;
    }
    if(gravityEnabled)
    {
      velocity.velocity.y += GRAVITY * 1.f/FPS * 1.f/FPS;
    }
  });
}


void applyVelocityToPosition(entt::registry& registry)
{
    auto view = registry.view<const Velocity, Box>();
    view.each([&](const Velocity& velocity, Box& box) 
        {
            box.center += velocity.velocity;
        });
}

void resetVerticalVelocityForEntitiesOnFloor(entt::registry& registry)
{
  auto view = registry.view<Velocity, const Adjacencies>();
  view.each([&](Velocity& velocity, const Adjacencies& adjacencies) 
  {
    if(adjacencies.isOnFloor)
    {
      velocity.velocity.y = 0;
    }
  });
}

void resetAdjacencies(entt::registry& registry)
{
  auto adjacencies = registry.view<Adjacencies>();
  adjacencies.each([&](Adjacencies& adjacencies) 
  {
    adjacencies.reset();
  });
}