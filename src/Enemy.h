#pragma once

#include "Entity.h"

#include <memory>

#include "Animation.h"

/**
* Type of enemy
*/
enum class EnemyType
{
    SkeletonBasic,
    GreenBlobBasic,
    BlueBlobBasic,
    RedBlobBasic,
    PinkBlobBasic,
    GhostBasic,
    MagicFlameBasic,
    AncientWizardBasic,
    DarkWizardBasic
};

/**
* Base enemy entity class
*/
class Enemy : public AliveEntity
{
public:
    Enemy();
    virtual ~Enemy();

    virtual void Render(sf::RenderTarget& target) override;

    virtual EnemyType GetEnemyType() const = 0;
};

/**
* Basic enemy types
*/
class BasicEnemy : public Enemy
{
    EnemyType enemyType_;

    Animation anim_;
    std::unique_ptr<AliveStats> stats_;

    sf::Time nextWanderMoveTime_;
    sf::Vector2f moveDir_;

    bool droppedItems_;

    void SetupAnimations();
    void NewWander();

    void HandleDropItems();

public:
    BasicEnemy(EnemyType enemyType);
    virtual ~BasicEnemy();

    virtual void ResetStats(float difficultyMul);
    void ResetStats();

    virtual void Tick() override;
    virtual void Render(sf::RenderTarget& target) override;

    virtual float GetAggroDistance() const;

    inline void SetHasDroppedItems(bool hasDropped) { droppedItems_ = hasDropped; }
    inline bool HasDroppedItems() const { return droppedItems_; }

    inline virtual EnemyType GetEnemyType() const override { return enemyType_; }

    virtual std::string GetUnitName() const override;

    inline virtual std::string GetName() const override { return "BasicEnemy"; }
};