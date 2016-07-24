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
* The weapon slot currently selected
*/
enum class PlayerSelectedWeapon
{
    MeleeWeapon,
    MagicWeapon
};

/**
* Struct storing the player inventory
*/
class PlayerInventory
{
    PlayerEntity* player_;

    PlayerSelectedWeapon selectedWeapon_;

    std::unique_ptr<MeleeWeapon> meleeWeapon_;
    std::unique_ptr<MagicWeapon> magicWeapon_;

    std::unique_ptr<PotionItem> healthPotions_;
    std::unique_ptr<PotionItem> magicPotions_;

public:
    PlayerInventory();
    ~PlayerInventory();

    inline void SetPlayer(PlayerEntity* player) { player_ = player; }

    inline PlayerEntity* GetPlayer() { return player_; }
    inline const PlayerEntity* GetPlayer() const { return player_; }

    inline void SetSelectedWeapon(PlayerSelectedWeapon selected) { selectedWeapon_ = selected; }
    inline PlayerSelectedWeapon GetSelectedWeapon() const { return selectedWeapon_; }

    bool CanGiveItem(Item* item) const;
    void GiveItem(Item* item);

    inline MeleeWeapon* GetMeleeWeapon() { return meleeWeapon_.get(); }
    inline const MeleeWeapon* GetMeleeWeapon() const { return meleeWeapon_.get(); }

    inline MagicWeapon* GetMagicWeapon() { return magicWeapon_.get(); }
    inline const MagicWeapon* GetMagicWeapon() const { return magicWeapon_.get(); }

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

    inline void SetInventory(PlayerInventory* inv) { if (inv) { inv_->SetPlayer(this); } inv_ = inv; }

    inline PlayerInventory* GetInventory() { return inv_; }
    inline const PlayerInventory* GetInventory() const { return inv_; }

    inline void SetUseRange(float useRange) { useRange_ = useRange; }
    inline float GetUseRange() const { return useRange_; }

    inline EntityId GetTargettedUsableEntity() const { return targettedUsableEnt_; }

    inline virtual std::string GetUnitName() const override { return "Player"; }

    inline virtual std::string GetName() const override { return "PlayerEntity"; }
};

