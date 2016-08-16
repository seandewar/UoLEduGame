#pragma once

#include <string>
#include <iostream>

#include <SFML/Graphics/RenderTarget.hpp>
#include <SFML/System/Time.hpp>

#include "Types.h"
#include "Animation.h"

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

    inline void MarkForDeletion()
    {
        if (!markedForDeletion_) {
            std::cout << "Marked for delete ent " << GetName() << " (ent id " << assignedId_ << ")\n";
            markedForDeletion_ = true;
        }
    }

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

    inline void SetCenterPosition(const sf::Vector2f& pos) { SetPosition(pos - GetSize() * 0.5f); }
    inline sf::Vector2f GetCenterPosition() const { return GetPosition() + GetSize() * 0.5f; }

    inline void Move(const sf::Vector2f& d) { rect_.left += d.x; rect_.top += d.y; }
    bool MoveWithCollision(const sf::Vector2f& d);
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
    u32 maxMana_, mana_;
    u32 meleeAttack_, magicAttack_;
    u32 meleeDefence_, magicDefence_;
    float moveSpeed_;

public:
    AliveStats();
    virtual ~AliveStats();

    inline virtual void SetHealth(u32 health) { health_ = std::min(maxHealth_, health); }
    inline virtual u32 GetHealth() const { return health_; }

    inline virtual bool IsAlive() const { return health_ > 0; }

    inline virtual void ApplyDamage(u32 damage) { health_ -= std::min(damage, health_); }
    inline virtual void ApplyHealing(u32 healAmount)
    {
        health_ += std::min(healAmount, UINT32_MAX - health_);
        health_ = std::min(maxHealth_, health_);
    }

    inline virtual void SetMaxHealth(u32 maxHealth) { maxHealth_ = maxHealth; health_ = std::min(maxHealth_, health_); }
    inline virtual u32 GetMaxHealth() const { return maxHealth_; }

    inline virtual void SetMana(u32 mana) { mana_ = std::min(maxMana_, mana); }
    inline virtual u32 GetMana() const { return mana_; }

    inline virtual void SetMaxMana(u32 maxMana) { maxMana_ = maxMana; mana_ = std::min(maxMana_, mana_); }
    inline virtual u32 GetMaxMana() const { return maxMana_; }

    inline virtual void SetMeleeAttack(u32 attack) { meleeAttack_ = attack; }
    inline virtual u32 GetMeleeAttack() const { return meleeAttack_; }

    inline virtual void SetMagicAttack(u32 attack) { magicAttack_ = attack; }
    inline virtual u32 GetMagicAttack() const { return magicAttack_; }

    inline virtual void SetMeleeDefence(u32 defence) { meleeDefence_ = defence; }
    inline virtual u32 GetMeleeDefence() const { return meleeDefence_; }

    inline virtual void SetMagicDefence(u32 defence) { magicDefence_ = defence; }
    inline virtual u32 GetMagicDefence() const { return magicDefence_; }

    inline virtual void SetMoveSpeed(float speed) { moveSpeed_ = speed; }
    inline virtual float GetMoveSpeed() const { return moveSpeed_; }
};

/**
* Different damage types
*/
enum class DamageType
{
    Melee,
    Magic,
    Other
};

/**
* Damage text ent
*/
class DamageTextEntity : public WorldEntity
{
    DamageType type_;
    u32 damage_;
    sf::Color color_;
    sf::Vector2f velo_;
    sf::Time timeLeft_;

public:
    DamageTextEntity(DamageType type, u32 damage, const sf::Color& color = sf::Color(255, 255, 255),
        const sf::Vector2f& velo = sf::Vector2f(0.0f, -15.0f), const sf::Time& displayTime = sf::seconds(1.0f));
    virtual ~DamageTextEntity();

    virtual void Tick() override;
    virtual void Render(sf::RenderTarget& target) override;

    inline DamageType GetDamageType() const { return type_; }
    inline u32 GetDamageAmount() const { return damage_; }
    inline sf::Vector2f GetVelocity() const { return velo_; }
    inline sf::Color GetTextColor() const { return color_; }

    inline virtual std::string GetName() const override { return "DamageTextEntity"; }
};

/**
* Different damage effects
*/
enum class DamageEffectType
{
    Zero,
    Flame,
    Drain,
    PlayerWave,
    EnemyWave,
    EnemyBlackFlame,
    EnemyMagicFlame,
    EnemySmoke
};

/**
* Damage effect ent
*/
class DamageEffectEntity : public WorldEntity
{
    DamageEffectType effectType_;
    Animation anim_;
    sf::Time timeLeft_;

    void SetupAnimations();

public:
    DamageEffectEntity(DamageEffectType type, const sf::Time& duration);
    virtual ~DamageEffectEntity();

    virtual void Tick() override;
    virtual void Render(sf::RenderTarget& target) override;

    inline DamageEffectType GetEffectType() const { return effectType_; }
    inline sf::Time GetTimeLeft() const { return timeLeft_; }

    inline virtual std::string GetName() const override { return "DamageEffectEntity"; }
};

/**
* Base class for an ent which has health and stats.
*/
class AliveEntity : public UnitEntity
{
    AliveStats* stats_;

public:
    AliveEntity();
    virtual ~AliveEntity();

    inline void SetStats(AliveStats* newStats) { stats_ = newStats; }

    inline AliveStats* GetStats() { return stats_; }
    inline const AliveStats* GetStats() const { return stats_; }

    virtual u32 Attack(u32 initialDamage, DamageType source = DamageType::Other);
    virtual u32 Damage(u32 amount, DamageType source = DamageType::Other);
    virtual void Heal(u32 amount);
};