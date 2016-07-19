#pragma once

#include "Entity.h"

/**
* Interface to be implemented by objects that wish to be usable
* by the player.
*/
class IPlayerUsable
{
public:
    IPlayerUsable() { }
    virtual ~IPlayerUsable() { }

    virtual void Use(EntityId playerId) = 0;

    virtual bool IsUsable() const = 0;
    virtual std::string GetUseText() const = 0;
};