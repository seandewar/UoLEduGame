#pragma once

#include "Entity.h"

#include "Item.h"
#include "Animation.h"

/**
* Entity where the player will be positioned if there are no other
* spawns available (e.g. root dir floor w/o a staircase to parent floor)
*/
class PlayerDefaultStartEntity : public WorldEntity
{
public:
    PlayerDefaultStartEntity();
    virtual ~PlayerDefaultStartEntity();

    inline virtual std::string GetName() const { return "PlayerDefaultStartEntity"; }
};

/**
* Struct storing the player inventory
*/
class PlayerInventory
{
    std::unique_ptr<PotionItem> healthPotions_;
    std::unique_ptr<PotionItem> magicPotions_;

public:
    PlayerInventory();
    ~PlayerInventory();

    bool CanGiveItem(Item* item) const;
    void GiveItem(Item* item);

    inline PotionItem* GetHealthPotions() { return healthPotions_.get(); }
    inline const PotionItem* GetHealthPotions() const { return healthPotions_.get(); }

    inline PotionItem* GetMagicPotions() { return magicPotions_.get(); }
    inline const PotionItem* GetMagicPotions() const { return magicPotions_.get(); }
};

/**
* The direction the player is facing
*/
enum class PlayerFacingDirection
{
    Up,
    Down,
    Left,
    Right
};

/**
* Player ent class.
*/
class PlayerEntity : public AliveEntity
{
    float useRange_;
    EntityId targettedUsableEnt_;

    PlayerInventory* inv_;

    PlayerFacingDirection dir_;

    Animation upAnim_;
    Animation downAnim_;
    Animation leftAnim_;
    Animation rightAnim_;

    void InitAnimations();

    void RestartAnimations();
    void TickAnimations();

    void HandleMovement();
    void HandleUseNearbyObjects();

public:
    PlayerEntity();
    virtual ~PlayerEntity();

    virtual void Tick() override;
    virtual void Render(sf::RenderTarget& target) override;

    virtual bool SetPositionToDefaultStart();

    inline void SetInventory(PlayerInventory* inv) { inv_ = inv; }

    inline PlayerInventory* GetInventory() { return inv_; }
    inline const PlayerInventory* GetInventory() const { return inv_; }

    inline void SetUseRange(float useRange) { useRange_ = useRange; }
    inline float GetUseRange() const { return useRange_; }

    inline EntityId GetTargettedUsableEntity() const { return targettedUsableEnt_; }

    inline virtual std::string GetUnitName() const override { return "Player"; }

    inline virtual std::string GetName() const override { return "PlayerEntity"; }
};

