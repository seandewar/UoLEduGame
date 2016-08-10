#pragma once

#include "Entity.h"

#include "Item.h"
#include "Animation.h"
#include "PlayerFacingDirection.h"

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
* The inv slots of the player
*/
enum class PlayerInventorySlot
{
    MeleeWeapon,
    MagicWeapon,
    HealthPotions,
    MagicPotions,
    Special
};

/**
* The type of selected weapon
*/
enum class PlayerSelectedWeapon
{
    Melee,
    Magic
};

/**
* Struct storing the player inventory
*/
class PlayerInventory
{
    friend class PlayerEntity;

    PlayerSelectedWeapon selectedWeapon_;

    std::unique_ptr<MeleeWeapon> meleeWeapon_;
    std::unique_ptr<MagicWeapon> magicWeapon_;

    std::unique_ptr<Armour> armour_;

    std::unique_ptr<PotionItem> healthPotions_;
    std::unique_ptr<PotionItem> magicPotions_;

public:
    PlayerInventory();
    ~PlayerInventory();

    inline void SetSelectedWeapon(PlayerSelectedWeapon selected) { selectedWeapon_ = selected; }
    inline PlayerSelectedWeapon GetSelectedWeapon() const { return selectedWeapon_; }

    inline BaseWeaponItem* GetWeaponItem(PlayerSelectedWeapon weapon)
    {
        switch (weapon) {
        case PlayerSelectedWeapon::Melee:
            return meleeWeapon_.get();

        case PlayerSelectedWeapon::Magic:
            return magicWeapon_.get();

        default:
            return nullptr;
        }
    }

    inline BaseWeaponItem* GetSelectedWeaponItem() { return GetWeaponItem(selectedWeapon_); }

    void GiveItem(Item* item);

    void TickUseDelays();

    inline MeleeWeapon* GetMeleeWeapon() { return meleeWeapon_.get(); }
    inline const MeleeWeapon* GetMeleeWeapon() const { return meleeWeapon_.get(); }

    inline MagicWeapon* GetMagicWeapon() { return magicWeapon_.get(); }
    inline const MagicWeapon* GetMagicWeapon() const { return magicWeapon_.get(); }

    inline Armour* GetArmour() { return armour_.get(); }
    inline const Armour* GetArmour() const { return armour_.get(); }

    inline PotionItem* GetHealthPotions() { return healthPotions_.get(); }
    inline const PotionItem* GetHealthPotions() const { return healthPotions_.get(); }

    inline PotionItem* GetMagicPotions() { return magicPotions_.get(); }
    inline const PotionItem* GetMagicPotions() const { return magicPotions_.get(); }
};

/**
* Player ent class.
*/
class PlayerEntity : public AliveEntity
{
    static const sf::Time attackAnimDuration_;

    float useRange_;
    EntityId targettedUsableEnt_;

    sf::Time invincibilityTime_;

    PlayerInventory* inv_;

    sf::Vector2f moveDir_;
    PlayerFacingDirection nextDir_;
    bool useTarget_;

    PlayerFacingDirection dir_;

    Animation upAnim_;
    Animation downAnim_;
    Animation leftAnim_;
    Animation rightAnim_;
    Animation deadAnim_;

    PlayerSelectedWeapon attackAnimWeapon_;
    sf::Time attackAnimTimeLeft_;

    bool handledDeath_;

    void InitAnimations();

    void RestartAnimations();
    void TickMoveAnimations();

    void HandleMovement();
    void HandleUseNearbyObjects();

public:
    PlayerEntity();
    virtual ~PlayerEntity();

    virtual void Tick() override;
    virtual void Render(sf::RenderTarget& target) override;

    void AddMoveInDirection(PlayerFacingDirection dir);

    inline void SetUseTargetThisFrame(bool useTarget) { useTarget_ = useTarget; }
    inline bool IsUsingTargetThisFrame() const { return useTarget_; }

    virtual u32 Attack(u32 initialDamage, DamageType source = DamageType::Other) override;

    inline virtual u32 Damage(u32 amount, DamageType source = DamageType::Other) override
    {
        if (invincibilityTime_ <= sf::Time::Zero) {
            invincibilityTime_ = sf::seconds(1.0f);
            return DamageWithoutInvincibility(amount, source);
        }

        return 0;
    }

    u32 DamageWithoutInvincibility(u32 amount, DamageType source = DamageType::Other);

    virtual bool SetPositionToDefaultStart();

    inline void SetInventory(PlayerInventory* inv) { inv_ = inv; }

    inline void ResetAttackAnimation() { attackAnimTimeLeft_ = sf::Time::Zero; }
    inline void PlayAttackAnimation(PlayerSelectedWeapon weapon)
    { 
        attackAnimWeapon_ = weapon;
        attackAnimTimeLeft_ = attackAnimDuration_;
    }

    void TickAttackAnimation();

    void UseInventorySlot(PlayerInventorySlot slot);
    bool CanPickupItem(Item* item) const;
    bool PickupItem(Item* item);

    inline void SetInvincibility(const sf::Time& invincibilityTime) { invincibilityTime_ = invincibilityTime; }
    inline sf::Time GetInvincibilityTimeLeft() const { return invincibilityTime_; }
    inline bool HasInvincibility() const { return invincibilityTime_ > sf::Time::Zero; }

    inline PlayerInventory* GetInventory() { return inv_; }
    inline const PlayerInventory* GetInventory() const { return inv_; }

    inline PlayerFacingDirection GetFacingDirection() const { return dir_; }

    inline void SetUseRange(float useRange) { useRange_ = useRange; }
    inline float GetUseRange() const { return useRange_; }

    inline EntityId GetTargettedUsableEntity() const { return targettedUsableEnt_; }

    inline virtual std::string GetUnitName() const override { return "Player"; }

    inline virtual std::string GetName() const override { return "PlayerEntity"; }
};

