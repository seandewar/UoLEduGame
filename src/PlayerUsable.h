#pragma once

#include "Entity.h"

/**
* Class to be implemented by objects that wish to be usable
* by the player.
*/
class PlayerUsable
{
public:
    PlayerUsable() { }
    virtual ~PlayerUsable() { }

    virtual void Use(EntityId playerId) = 0;

    virtual bool IsUsable(EntityId playerId) const = 0;
    virtual std::string GetUseText() const = 0;
    virtual std::string GetCannotUseText() const { return std::string(); }
};