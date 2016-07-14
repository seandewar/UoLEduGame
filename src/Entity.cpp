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
moveSpeed_(100.0f)
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