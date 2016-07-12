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


AliveEntity::AliveEntity() :
UnitEntity(),
maxHealth_(1),
health_(1)
{
}


AliveEntity::~AliveEntity()
{
}