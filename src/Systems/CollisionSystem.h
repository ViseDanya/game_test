#include <entt/entt.hpp>
#include "Components/Box.h"
#include "Components/Velocity.h"
#include "Components/Adjacencies.h"
#include "Components/Mass.h"
#include "Components/Trampoline.h"
#include "Components/Conveyor.h"
#include <numeric>
#include <iostream>

constexpr int MAX_COLLISION_ITERATIONS = 100;
const float COLLISION_TOLERANCE = .001f;

struct CollisionInfo
{
    entt::entity e1;
    entt::entity e2;

    Direction direction;
    float overlap;

    CollisionInfo reverse()
    {
        return CollisionInfo{e2, e1, getOppositeDirection(direction), overlap};
    }
};

// this function assumes the boxes are overlapping already
std::tuple<Direction, float> getCollisionDirectionAndOverlap(const Box& box1, const Box& box2)
{
    const float leftOverlap = box2.right() - box1.left();
    const float rightOverlap = box1.right() - box2.left();
    const float downOverlap = box2.top() - box1.bottom();
    const float upOverlap = box1.top() - box2.bottom();
    const float minimumOverlap = std::min({leftOverlap, rightOverlap, downOverlap, upOverlap});
    if(minimumOverlap == leftOverlap)
    {
        return std::tuple(Direction::LEFT, minimumOverlap);
    }
    else if(minimumOverlap == rightOverlap)
    {
        return std::tuple(Direction::RIGHT, minimumOverlap);
    }
    else if(minimumOverlap == downOverlap)
    {
        return std::tuple(Direction::DOWN, minimumOverlap);
    }
    else
    {
        return std::tuple(Direction::UP, minimumOverlap);
    }
}

void resolveDynamicWithDynamicCollision(entt::registry& registry, CollisionInfo collisionInfo)
{
    std::cout << "resolveDynamicWithDynamicCollision" << std::endl;
    auto [box1, adjacencies1, mass1] = registry.get<Box, Adjacencies, const Mass>(collisionInfo.e1);
    auto [box2, adjacencies2, mass2] = registry.get<Box, Adjacencies, const Mass>(collisionInfo.e2);

    const Direction collisionDirection = collisionInfo.direction;
    const Direction collisionOppositeDirection = getOppositeDirection(collisionDirection);
    const glm::vec2 collisionDirectionVector = getDirectionAsVector(collisionDirection);
    const glm::vec2 collisionOppositeDirectionVector = getDirectionAsVector(collisionOppositeDirection); 
    std::vector<entt::entity> adjacenciesInDirectionOfCollision = adjacencies2.getAdjacencyList(registry, collisionDirection);
    std::vector<entt::entity> adjacenciesInOppositeDirectionOfCollision = adjacencies1.getAdjacencyList(registry, collisionOppositeDirection);

    if (adjacencies1.getIsOnStaticObject(collisionOppositeDirection))
    {
        std::cout << "resolveDynamicWithDynamicCollision e1 on static object" << std::endl;

        adjacencies2.setIsOnStaticObject(collisionOppositeDirection, true);
        box2.center += collisionDirectionVector * (collisionInfo.overlap + COLLISION_TOLERANCE);
        for(entt::entity adjacentEntityInDirectionOfCollision : adjacenciesInDirectionOfCollision)
        {
            Adjacencies& adjacencies = registry.get<Adjacencies>(adjacentEntityInDirectionOfCollision);
            adjacencies.setIsOnStaticObject(collisionOppositeDirection, true);
            Box& box = registry.get<Box>(adjacentEntityInDirectionOfCollision);
            box.center += collisionDirectionVector * (collisionInfo.overlap + COLLISION_TOLERANCE);
        }
    }
    else if(adjacencies2.getIsOnStaticObject(collisionDirection))
    {
        std::cout << "resolveDynamicWithDynamicCollision e2 on static object" << std::endl;

        adjacencies1.setIsOnStaticObject(collisionDirection, true);
        box1.center += collisionOppositeDirectionVector * (collisionInfo.overlap + COLLISION_TOLERANCE);
        for(entt::entity adjacentEntityInOppositeDirectionOfCollision : adjacenciesInOppositeDirectionOfCollision)
        {
            Adjacencies& adjacencies = registry.get<Adjacencies>(adjacentEntityInOppositeDirectionOfCollision);
            adjacencies.setIsOnStaticObject(collisionDirection, true);
            Box& box = registry.get<Box>(adjacentEntityInOppositeDirectionOfCollision);
            box.center += collisionOppositeDirectionVector * (collisionInfo.overlap + COLLISION_TOLERANCE);
        }
    }
    else
    {
        std::cout << "dynamic collision" << std::endl;

        const float totalMassInDirectionOfCollision = 
        std::accumulate(adjacenciesInDirectionOfCollision.cbegin(), adjacenciesInDirectionOfCollision.cend(), mass2.mass, 
          [&](float acc, entt::entity e){return acc + registry.get<const Mass>(e).mass;});
        const float totalMassInOppositeDirectionOfCollision = 
        std::accumulate(adjacenciesInOppositeDirectionOfCollision.cbegin(), adjacenciesInOppositeDirectionOfCollision.cend(), mass1.mass, 
          [&](float acc, entt::entity e){return acc + registry.get<const Mass>(e).mass;});
        const float totalMass = totalMassInDirectionOfCollision + totalMassInOppositeDirectionOfCollision;

        const glm::vec2 offsetInOppositeDirectionOfCollision = 
            collisionOppositeDirectionVector * (collisionInfo.overlap * totalMassInDirectionOfCollision / totalMass + COLLISION_TOLERANCE);
        box1.center += offsetInOppositeDirectionOfCollision;
        const glm::vec2 offsetInDirectionOfCollision = 
            collisionDirectionVector * (collisionInfo.overlap * totalMassInOppositeDirectionOfCollision / totalMass + COLLISION_TOLERANCE); 
        box2.center += offsetInDirectionOfCollision;

        std::cout << "offsetInOppositeDirectionOfCollision: " << offsetInOppositeDirectionOfCollision << std::endl;
        std::cout << "offsetInDirectionOfCollision: " << offsetInDirectionOfCollision << std::endl;

        std::for_each(adjacenciesInOppositeDirectionOfCollision.cbegin(), adjacenciesInOppositeDirectionOfCollision.cend(), 
        [&](entt::entity e){registry.get<Box>(e).center += offsetInOppositeDirectionOfCollision;});
        std::for_each(adjacenciesInDirectionOfCollision.cbegin(), adjacenciesInDirectionOfCollision.cend(), 
        [&](entt::entity e){registry.get<Box>(e).center += offsetInDirectionOfCollision;});
    // downBox.center -= glm::UP * (overlap + COLLISION_TOLERANCE);
    // Velocity& upVelocity = registry.get<Velocity>(upObject);
    // Velocity& downVelocity = registry.get<Velocity>(downObject);
    // upVelocity.velocity.y = downVelocity.velocity.y;
    // downVelocity.velocity.y = 0;
    }

    adjacencies1.adjacencies[collisionDirection] = collisionInfo.e2;
    adjacencies2.adjacencies[collisionOppositeDirection] = collisionInfo.e1;
}

void resolveDynamicWithTrampolineCollision(entt::registry& registry, CollisionInfo collisionInfo)
{
    auto [velocity, adjacencies] = registry.get<Velocity, Adjacencies>(collisionInfo.e1);
    auto trampoline = registry.get<Trampoline>(collisionInfo.e2);
    if(collisionInfo.direction == Direction::DOWN)
    {
        adjacencies.isOnFloor = false;
        velocity.velocity.y = trampoline.impulse * 1./FPS;
    }
}

void resolveDynamicWithConveyorCollision(entt::registry& registry, CollisionInfo collisionInfo)
{
    auto [box, adjacencies] = registry.get<Box, Adjacencies>(collisionInfo.e1);
    auto conveyor = registry.get<Conveyor>(collisionInfo.e2);
    if(collisionInfo.direction == Direction::DOWN)
    {
        box.center.x += conveyor.speed * 1./FPS;
    }
}

void resolveDynamicWithStaticCollision(entt::registry& registry, CollisionInfo collisionInfo)
{
    auto [box, velocity, adjacencies] = registry.get<Box, Velocity, Adjacencies>(collisionInfo.e1);
    const Direction collisionDirection = collisionInfo.direction;
    const Direction collisionOppositeDirection = getOppositeDirection(collisionDirection);
    const glm::vec2 collisionDirectionVector = getDirectionAsVector(collisionDirection);
    const glm::vec2 collisionOppositeDirectionVector = getDirectionAsVector(collisionOppositeDirection);
    adjacencies.setIsOnStaticObject(collisionDirection, true);
    const glm::vec2 collisionOffset = collisionOppositeDirectionVector * (collisionInfo.overlap + COLLISION_TOLERANCE);
    box.center += collisionOffset;
    std::vector<entt::entity> adjacenciesInOppositeDirectionOfCollision = adjacencies.getAdjacencyList(registry, collisionOppositeDirection);
    for(entt::entity adjacentEntityInOppositeDirectionOfCollision : adjacenciesInOppositeDirectionOfCollision)
    {
        Adjacencies& adjacencies = registry.get<Adjacencies>(adjacentEntityInOppositeDirectionOfCollision);
        adjacencies.setIsOnStaticObject(collisionDirection, true);
        Box& box = registry.get<Box>(adjacentEntityInOppositeDirectionOfCollision);
        box.center += collisionOppositeDirectionVector * (collisionInfo.overlap + COLLISION_TOLERANCE);
    }
}


void resolveCollision(entt::registry& registry, CollisionInfo collisionInfo)
{
    const bool isFirstEntityDynamic = registry.all_of<Box, Velocity, Mass, Adjacencies>(collisionInfo.e1);
    const bool isSecondEntityDynamic = registry.all_of<Box, Velocity, Mass, Adjacencies>(collisionInfo.e2);
    if(isFirstEntityDynamic && isSecondEntityDynamic)
    {
        resolveDynamicWithDynamicCollision(registry, collisionInfo);
    }
    else if(isFirstEntityDynamic)
    {
        resolveDynamicWithStaticCollision(registry, collisionInfo);
        if(registry.all_of<Trampoline>(collisionInfo.e2))
        {
            resolveDynamicWithTrampolineCollision(registry, collisionInfo);
        }
        if(registry.all_of<Conveyor>(collisionInfo.e2))
        {
            resolveDynamicWithConveyorCollision(registry, collisionInfo);
        }
    }
    else if(isSecondEntityDynamic)
    {
        resolveDynamicWithStaticCollision(registry, collisionInfo.reverse());
        if(registry.all_of<Trampoline>(collisionInfo.e1))
        {
            resolveDynamicWithTrampolineCollision(registry, collisionInfo.reverse());
        }
        if(registry.all_of<Conveyor>(collisionInfo.e1))
        {
            resolveDynamicWithConveyorCollision(registry, collisionInfo.reverse());
        }
    }
}

void resolveCollisions(entt::registry& registry)
{
    bool collisionDetected = true;
    int numCollisionIterations = 0;
    auto collidableEntities = registry.view<Box>();
    while(collisionDetected && numCollisionIterations < MAX_COLLISION_ITERATIONS)
    {
        collisionDetected = false;
        collidableEntities.each([&](const entt::entity e1, Box& box1)
        {
            collidableEntities.each([&](const entt::entity e2, Box& box2)
            {
                if(e1 != e2 && box1.intersects(box2))
                {
                    collisionDetected =  true;
                    CollisionInfo collisionInfo;
                    collisionInfo.e1 = e1;
                    collisionInfo.e2 = e2;
                    const std::tuple<Direction, float> collisionDirectionAndOverlap = getCollisionDirectionAndOverlap(box1, box2);
                    collisionInfo.direction = std::get<0>(collisionDirectionAndOverlap);
                    collisionInfo.overlap = std::get<1>(collisionDirectionAndOverlap);
                    std::cout << "Colllision overlap:" << collisionInfo.overlap << std::endl;
                    std::cout << "Collision direction: " << collisionInfo.direction << std::endl;
                    resolveCollision(registry, collisionInfo);
                }
            });
        });
        numCollisionIterations++;
    }
    std::cout << "numCollisionIterations: " << numCollisionIterations << std::endl;
}


