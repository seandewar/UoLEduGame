#pragma once

#include <string>

#include <SFML/Graphics/RenderTarget.hpp>

#include "Types.h"

typedef u64 EntityId;

class WorldArea;

/**
* Base entity class.
*/
class Entity
{
    // WorldArea needs to be able to set assignedId_ and assignedArea_
    friend class WorldArea;

    EntityId assignedId_;
    WorldArea* assignedArea_;
    bool markedForDeletion_;

public:
    static const EntityId InvalidId = UINT64_MAX;

    Entity();
    virtual ~Entity();

    inline virtual void Tick() { }
    inline virtual void Render(sf::RenderTarget& target) { }

    inline void MarkForDeletion() { markedForDeletion_ = true; }
    inline bool IsMarkedForDeletion() const { return markedForDeletion_; }

    inline EntityId GetAssignedId() const { return assignedId_; }
    inline WorldArea* GetAssignedArea() const { return assignedArea_; }

    virtual std::string GetName() const = 0;
};

/**
* Base class for an ent inside of the world with a pos and size.
*/
class WorldEntity : public Entity
{
    sf::FloatRect rect_;

public:
    WorldEntity();
    virtual ~WorldEntity();

    inline void SetRectangle(const sf::FloatRect& rect) { rect_ = rect; }
    inline sf::FloatRect GetRectangle() const { return rect_; }

    inline void SetPosition(const sf::Vector2f& pos) { rect_.left = pos.x; rect_.top = pos.y; }
    inline sf::Vector2f GetPosition() const { return sf::Vector2f(rect_.left, rect_.top); }

    inline void SetSize(const sf::Vector2f& size) { rect_.width = size.x; rect_.height = size.y; }
    inline sf::Vector2f GetSize() const { return sf::Vector2f(rect_.width, rect_.height); }

    inline sf::Vector2f GetCenterPosition() const { return GetPosition() + (GetSize() * 0.5f); }
};

/**
* Base class for an interactable ent in the world.
*/
class UnitEntity : public WorldEntity
{
public:
    UnitEntity();
    virtual ~UnitEntity();

    virtual std::string GetUnitName() const = 0;
};

/**
* Stats of an AliveEnt.
*/
class AliveStats
{
    u32 maxHealth_, health_;
    float moveSpeed_;

public:
    AliveStats();
    virtual ~AliveStats();

    inline virtual void SetHealth(u32 health) { health_ = std::min(maxHealth_, health); }
    inline virtual u32 GetHealth() const { return health_; }

    inline virtual bool IsAlive() const { return health_ > 0; }

    inline virtual void SetMaxHealth(u32 maxHealth) { maxHealth_ = maxHealth; health_ = std::min(maxHealth_, health_); }
    inline virtual u32 GetMaxHealth() const { return maxHealth_; }

    inline virtual void ApplyDamage(u32 damage) { health_ -= std::min(damage, health_); }

    inline virtual void ApplyHealing(u32 healAmount)
    {
        health_ += std::min(healAmount, UINT32_MAX - health_);
        health_ = std::min(maxHealth_, health_);
    }

    inline virtual void SetMoveSpeed(float speed) { moveSpeed_ = speed; }
    inline virtual float GetMoveSpeed() const { return moveSpeed_; }
};

/**
* Base class for an ent which has health and stats.
*/
class AliveEntity : public UnitEntity
{
    AliveStats stats_;

public:
    AliveEntity();
    virtual ~AliveEntity();

    inline AliveStats& GetStats() { return stats_; }
    inline const AliveStats& GetStats() const { return stats_; }
};