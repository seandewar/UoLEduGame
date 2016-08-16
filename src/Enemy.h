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
    DarkWizardBasic,
    DungeonGuardian
};

/**
* Base enemy entity class
*/
class Enemy : public AliveEntity
{
public:
    Enemy();
    virtual ~Enemy();

    virtual u32 Damage(u32 damageAmount, DamageType type) override;

    virtual void Render(sf::RenderTarget& target) override;

    virtual EnemyType GetEnemyType() const = 0;
};

/**
* The different forms that the Dungeon Guardian can take during battle
*/
enum class DungeonGuardianForm
{
    MagicForm,
    SmokeForm,
    MeleeForm
};

/**
* Dungeon Guardian boss
*/
class DungeonGuardian : public Enemy
{
    std::unique_ptr<AliveStats> stats_;

    DungeonGuardianForm form_;
    int formActionsLeft_;
    sf::Time actionTimeLeft_;
    sf::Time formSoundTimeLeft_;
    bool firedThisAction_;
    bool handleDeathAnim_;

    sf::Vector2f moveDir_;

    Animation animMagicForm_;
    Animation animSmokeForm_;
    Animation animMeleeForm_;

    float rot_;

    virtual void SetupAnimations();

    virtual sf::Time GetDefaultTimeForAction();

    inline virtual void NewFormAction() { actionTimeLeft_ = GetDefaultTimeForAction(); firedThisAction_ = false; }

    virtual void ChangeForm(DungeonGuardianForm newForm);

    virtual void ResetAnimations();
    virtual void TickAnimations();

public:
    DungeonGuardian();
    virtual ~DungeonGuardian();

    void PlayerKilled();

    virtual void ResetStats(float difficultyMul);
    void ResetStats();

    virtual void Tick() override;
    virtual void Render(sf::RenderTarget& target) override;

    inline DungeonGuardianForm GetCurrentForm() const { return form_; }

    inline virtual EnemyType GetEnemyType() const override { return EnemyType::DungeonGuardian; }

    inline virtual std::string GetUnitName() const override { return "Dungeon Guardian"; }
    inline virtual std::string GetName() const override { return "DungeonGuardian"; }
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