#pragma once

#include "Entity.h"

/**
* Type of projectile
*/
enum class ProjectileType
{
    PlayerMagicWave,
    EnemyMagicWave,
    EnemyMagicFlame,
    EnemySmoke,
    EffectOrb
};

/**
* Projectile class
*/
class ProjectileEntity : public UnitEntity
{
    ProjectileType projectileType_;

    Animation anim_;

    u32 damage_;
    sf::Vector2f velo_;
    sf::Time timeLeft_;

    void SetupAnimations();

public:
    ProjectileEntity(ProjectileType projectileType, const sf::Vector2f& dir);
    virtual ~ProjectileEntity();

    virtual void Tick() override;
    virtual void Render(sf::RenderTarget& target) override;

    inline void SetVelocity(const sf::Vector2f& velo) { velo_ = velo; }
    inline sf::Vector2f GetVelocity() const { return velo_; }

    inline ProjectileType GetProjectileType() const { return projectileType_; }

    bool IsPlayerProjectile() const;
    bool CollidesWithTiles() const;

    inline void SetDamage(u32 damage) { damage_ = damage; }
    inline u32 GetDamage() const { return damage_; }

    inline sf::Time GetTimeLeft() const { return timeLeft_; }

    inline virtual std::string GetUnitName() const override { return "Projectile"; }
    inline virtual std::string GetName() const override { return "ProjectileEntity"; }
};
