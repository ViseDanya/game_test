#include "CollisionSystem.h"
#include <entt/entt.hpp>
#include "Components/Box.h"
#include "Components/Velocity.h"
#include "Components/Adjacencies.h"
#include "Components/Mass.h"
#include "Components/Trampoline.h"
#include "Components/Conveyor.h"
#include "Components/Collider.h"
#include "Components/Fake.h"
#include "Components/Health.h"
#include "Components/HealthChanger.h"
#include "Components/Ceiling.h"
#include "Components/Type.h"
#include "Constants.h"
#include <numeric>
#include <iostream>

static constexpr int MAX_COLLISION_ITERATIONS = 100;
static const float COLLISION_TOLERANCE = .001f;

struct pair_hash {
    template <typename T, typename U>
    std::size_t operator ()(const std::pair<T, U>& p) const {
        auto hash1 = std::hash<T>{}(p.first); // Hash for the first element
        auto hash2 = std::hash<U>{}(p.second); // Hash for the second element
        
        // Combine the two hash values (using a common trick)
        return hash1 ^ (hash2 << 1);  // XOR and shift
    }
};

struct CollisionManager
{
    std::unordered_set<std::pair<entt::entity,entt::entity>, pair_hash> previousFrameCollisions;
    std::unordered_set<std::pair<entt::entity,entt::entity>, pair_hash> currentFrameCollisions;
    std::unordered_set<std::pair<entt::entity,entt::entity>, pair_hash> ignoreCollisions;

    void reset()
    {
        updateIgnoreCollisions();
        previousFrameCollisions = std::move(currentFrameCollisions);
        currentFrameCollisions.clear();
    }

    void registerCollision(entt::entity e1, entt::entity e2)
    {
        currentFrameCollisions.insert(getCollisionPair(e1,e2));
    }

    bool areEntitiesCollisingThisFrame(entt::entity e1, entt::entity e2)
    {
        return currentFrameCollisions.count(getCollisionPair(e1,e2)) > 0;
    }

    bool wereEntitesCollidingInPreviousFrame(entt::entity e1, entt::entity e2)
    {
        return previousFrameCollisions.count(getCollisionPair(e1,e2)) > 0;
    }

    void ignoreCollision(entt::entity e1, entt::entity e2)
    {
        ignoreCollisions.insert(getCollisionPair(e1,e2));
    }

    bool shouldIgnoreCollision(entt::entity e1, entt::entity e2)
    {
        return ignoreCollisions.count(getCollisionPair(e1,e2)) > 0;
    }

    void updateIgnoreCollisions()
    {
        for (auto it = ignoreCollisions.begin(); it != ignoreCollisions.end(); ) {
            if (currentFrameCollisions.count(*it) == 0) {
                it = ignoreCollisions.erase(it);
            }
            else {
                ++it;
            }
        }
    }

    std::pair<entt::entity, entt::entity> getCollisionPair(entt::entity e1, entt::entity e2)
    {
        if(e1 < e2)
        {
            return std::pair(e1, e2);
        }
        else
        {
            return std::pair(e2, e1);
        }
    }
};

static CollisionManager collisionManager;

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
std::pair<Direction, float> getCollisionDirectionAndOverlap(const Box& box1, const Box& box2)
{
    const float leftOverlap = box2.right() - box1.left();
    const float rightOverlap = box1.right() - box2.left();
    const float downOverlap = box2.top() - box1.bottom();
    const float upOverlap = box1.top() - box2.bottom();
    const float minimumOverlap = std::min({leftOverlap, rightOverlap, downOverlap, upOverlap});
    if(minimumOverlap == leftOverlap)
    {
        return std::pair(Direction::LEFT, minimumOverlap);
    }
    else if(minimumOverlap == rightOverlap)
    {
        return std::pair(Direction::RIGHT, minimumOverlap);
    }
    else if(minimumOverlap == downOverlap)
    {
        return std::pair(Direction::DOWN, minimumOverlap);
    }
    else
    {
        return std::pair(Direction::UP, minimumOverlap);
    }
}

void resolveDynamicWithDynamicCollision(entt::registry& registry, CollisionInfo collisionInfo)
{
    // std::cout << "resolveDynamicWithDynamicCollision" << std::endl;
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
        // std::cout << "resolveDynamicWithDynamicCollision e1 on static object" << std::endl;

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
        // std::cout << "resolveDynamicWithDynamicCollision e2 on static object" << std::endl;

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
        // std::cout << "dynamic collision" << std::endl;

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

        // std::cout << "offsetInOppositeDirectionOfCollision: " << offsetInOppositeDirectionOfCollision << std::endl;
        // std::cout << "offsetInDirectionOfCollision: " << offsetInDirectionOfCollision << std::endl;

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

    if(collisionDirection == Direction::UP)
    {
        auto& v1 = registry.get<Velocity>(collisionInfo.e1);
        auto& v2 = registry.get<Velocity>(collisionInfo.e2);
        v2.velocity.y = v1.velocity.y;
        v1.velocity.y = 0;
    }
    else if(collisionDirection == Direction::DOWN)
    {
        auto& v1 = registry.get<Velocity>(collisionInfo.e1);
        auto& v2 = registry.get<Velocity>(collisionInfo.e2);
        v1.velocity.y = v2.velocity.y;
        v2.velocity.y = 0;
    }

    adjacencies1.adjacencies[collisionDirection] = collisionInfo.e2;
    adjacencies2.adjacencies[collisionOppositeDirection] = collisionInfo.e1;
}

void resolveDynamicWithTrampolineCollision(entt::registry& registry, CollisionInfo collisionInfo)
{
    auto [velocity, adjacencies] = registry.get<Velocity, Adjacencies>(collisionInfo.e1);
    auto& trampoline = registry.get<Trampoline>(collisionInfo.e2);
    if(collisionInfo.direction == Direction::DOWN)
    {
        trampoline.state = Trampoline::State::TRIGGERED;
        adjacencies.isOnFloor = false;
        velocity.velocity.y = trampoline.impulse * 1.f/FPS;
    }
}

void resolveDynamicWithConveyorCollision(entt::registry& registry, CollisionInfo collisionInfo)
{
    auto& box = registry.get<Box>(collisionInfo.e1);
    const auto& conveyor = registry.get<Conveyor>(collisionInfo.e2);
    box.center.x += conveyor.speed * 1.f/FPS;
}

void resolveDynamicWithFakeCollision(entt::registry& registry, CollisionInfo collisionInfo)
{
    auto& fake = registry.get<Fake>(collisionInfo.e2);
    if(fake.state == Fake::State::IDLE && collisionInfo.direction == Direction::DOWN)
    {
        fake.state = Fake::State::TRIGGERED;
        fake.collisionTime = SDL_GetTicks();
    }
}

void resolveHealthWithHealthChangerCollision(entt::registry& registry, CollisionInfo collisionInfo)
{
    auto& health = registry.get<Health>(collisionInfo.e1);
    auto& healthChanger = registry.get<HealthChanger>(collisionInfo.e2);
    if(!collisionManager.wereEntitesCollidingInPreviousFrame(collisionInfo.e1, collisionInfo.e2))
    {
        health.applyChangeToHealth(healthChanger.amount);
    }
}

void resolveDynamicWithCeilingCollision(entt::registry& registry, CollisionInfo collisionInfo)
{
    auto& adjacencies = registry.get<Adjacencies>(collisionInfo.e1);
    if(adjacencies.isOnFloor)
    {
        std::cout << "isOnFloor" << std::endl;
        entt::entity floorEntity = adjacencies.adjacencies[Direction::DOWN];
        const auto& type = registry.get<TypeComponent>(floorEntity);
        if(type.isPlatform())
        {
            std::cout << "ignoreCollision" << std::endl;
            collisionManager.ignoreCollision(collisionInfo.e1, floorEntity);
        }
    }
}

void resolveDynamicWithStaticCollision(entt::registry& registry, CollisionInfo collisionInfo)
{
    std::cout << "resolve dynamic with static lol " << entt::to_integral(collisionInfo.e2) << std::endl;

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
        if(registry.all_of<Adjacencies>(adjacentEntityInOppositeDirectionOfCollision))
        {
            Adjacencies& adjacencies = registry.get<Adjacencies>(adjacentEntityInOppositeDirectionOfCollision);
            adjacencies.setIsOnStaticObject(collisionDirection, true);
            Box& box = registry.get<Box>(adjacentEntityInOppositeDirectionOfCollision);
            box.center += collisionOffset;
        }
    }
    adjacencies.adjacencies[collisionDirection] = collisionInfo.e2;
    std::cout << "resolve dynamic with static " << entt::to_integral(collisionInfo.e2) << std::endl;
}

void resolvePhysicsCollision(entt::registry& registry, CollisionInfo collisionInfo)
{
    const bool isFirstEntityDynamic = registry.all_of<Box, Velocity, Mass, Adjacencies>(collisionInfo.e1);
    const bool isSecondEntityDynamic = registry.all_of<Box, Velocity, Mass, Adjacencies>(collisionInfo.e2);
    if(isFirstEntityDynamic)
    {
        if(isSecondEntityDynamic)
        {
            resolveDynamicWithDynamicCollision(registry, collisionInfo);
        }
        else
        {
            const auto& collider = registry.get<Collider>(collisionInfo.e2);
            if(!collider.isTrigger)
            {
                resolveDynamicWithStaticCollision(registry, collisionInfo);
            }
        }
        if(registry.all_of<Trampoline>(collisionInfo.e2))
        {
            resolveDynamicWithTrampolineCollision(registry, collisionInfo);
        }
        if(registry.all_of<Ceiling>(collisionInfo.e2))
        {
            resolveDynamicWithCeilingCollision(registry, collisionInfo);
        }
        if(registry.all_of<Fake>(collisionInfo.e2))
        {
            resolveDynamicWithFakeCollision(registry, collisionInfo);
        }
    }
    if(registry.all_of<Health>(collisionInfo.e1) && registry.all_of<HealthChanger>(collisionInfo.e2))
    {
        resolveHealthWithHealthChangerCollision(registry, collisionInfo);
    }
}

template<typename V1, typename V2>
bool resolveCollision(V1 view1, V2 view2, std::function<void(entt::registry&, CollisionInfo)> collisionResolutionFunction, entt::registry& registry)
{
    bool collisionDetected = false;
    for(const entt::entity e1 : view1)
    {
        Box& box1 = registry.get<Box>(e1);
        Velocity& velocity1 = registry.get<Velocity>(e1);
        Collider& collider1 = registry.get<Collider>(e1);
        for(const entt::entity e2 : view2)
        {
            Box& box2 = registry.get<Box>(e2);
            Collider& collider2 = registry.get<Collider>(e2);
            Box collider1InWorldSpace = Box(collider1.box.center + box1.center, collider1.box.size);
            Box collider2InWorldSpace = Box(collider2.box.center + box2.center, collider2.box.size);
            if(e1 != e2 && collider1.isEnabled && collider2.isEnabled &&
                collider1InWorldSpace.intersects(collider2InWorldSpace))
            {
                CollisionInfo collisionInfo;
                collisionInfo.e1 = e1;
                collisionInfo.e2 = e2;
                const std::pair<Direction, float> collisionDirectionAndOverlap = 
                    getCollisionDirectionAndOverlap(collider1InWorldSpace, collider2InWorldSpace);
                collisionInfo.direction = collisionDirectionAndOverlap.first;
                collisionInfo.overlap = collisionDirectionAndOverlap.second;
                // std::cout << "Collision overlap:" << collisionInfo.overlap << std::endl;
                // std::cout << "Collision direction: " << collisionInfo.direction << std::endl;
                if(collider2.isOneWay && (collisionInfo.direction != Direction::DOWN || velocity1.velocity.y >= 0))
                {
                    continue;
                }
                else
                {
                    collisionManager.registerCollision(e1, e2);
                    if(!collisionManager.shouldIgnoreCollision(e1, e2))
                    {
                        collisionDetected = true;
                        collisionResolutionFunction(registry, collisionInfo);
                    }
                }
            }
        }
    }
    return collisionDetected;
}

void resolvePrePhysicsCollisions(entt::registry& registry)
{
    // conveyor collisions need to be resolved before physics because they move the players
    auto movingCollidableEntities = registry.view<Box, Velocity, Collider>();
    auto conveyorEntities = registry.view<Box, Collider, Conveyor>();
    resolveCollision(movingCollidableEntities, conveyorEntities, resolveDynamicWithConveyorCollision, registry);
}

void resolvePostPhysicsCollisions(entt::registry& registry)
{
    // never mind, the ceiling collisions need to be done during the physics step, because the ceiling is also
    // a static obect. When a player is squashed between the ceiling and a platform, the ceiling collision needs
    // to run so that the player can fall through the platform rather than running the collision logic for max iterations
    
    // ceiling collisions need to be resolved after physics because we need to know if the player is on a platform
    // auto movingCollidableEntities = registry.view<Box, Velocity, Collider>();
    // auto ceilingEntites = registry.view<Box, Collider, Ceiling>();
    // resolveCollision(movingCollidableEntities, ceilingEntites, resolveDynamicWithCeilingCollision, registry);
}

void resolvePhysicsCollisions(entt::registry& registry)
{
    auto movingCollidableEntites = registry.view<Box, Velocity, Collider>();
    auto collidableEntities = registry.view<Box, Collider>();
    bool collisionDetected = true;
    int numCollisionIterations = 0;
    while(collisionDetected && numCollisionIterations < MAX_COLLISION_ITERATIONS)
    {
        collisionDetected = resolveCollision(movingCollidableEntites, collidableEntities, resolvePhysicsCollision, registry);
        numCollisionIterations++;
    }
    // std::cout << "numCollisionIterations: " << numCollisionIterations << std::endl;
}

void resolveCollisions(entt::registry& registry)
{
    collisionManager.reset();
    resolvePrePhysicsCollisions(registry);
    resolvePhysicsCollisions(registry);
    resolvePostPhysicsCollisions(registry);
}