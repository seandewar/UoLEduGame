#include "Entity.h"

#include "World.h"


Entity::Entity() :
assignedId_(Entity::InvalidId),
assignedArea_(nullptr),
markedForDeletion_(false)
{
}


Entity::~Entity()
{
}


WorldEntity::WorldEntity() :
Entity()
{
}


WorldEntity::~WorldEntity()
{
}


void WorldEntity::MoveWithCollision(const sf::Vector2f& d)
{
    auto area = GetAssignedArea();
    if (area) {
        sf::Vector2f dMove;
        area->TryCollisionRectMove(GetRectangle(), d, &dMove);

        Move(dMove);
    }
}


UnitEntity::UnitEntity() :
WorldEntity()
{
}


UnitEntity::~UnitEntity()
{
}


AliveStats::AliveStats() :
maxHealth_(1),
health_(1),
moveSpeed_(1.0f)
{
}


AliveStats::~AliveStats()
{
}


AliveEntity::AliveEntity() :
UnitEntity()
{
}


AliveEntity::~AliveEntity()
{
}