#pragma once

#include <string>
#include <memory>

#include <SFML/System/Time.hpp>
#include <SFML/Graphics/RenderTarget.hpp>
#include <SFML/Graphics/Sprite.hpp>

#include "Entity.h"
#include "IPlayerUsable.h"

/**
* The type of item
*/
enum class ItemType
{
    HealthPotion,
    MagicPotion,
    MeleeWeapon,
    MagicWeapon,
    ArtefactPiece
};

class PlayerEntity;

/**
* Base item class
*/
class Item
{
    int amount_, maxAmount_;
    sf::Time useDelayLeft_;

public:
    Item(int maxAmount = 1, int amount = 0);
    virtual ~Item();

    /**
    * Returns the amount of items actually added
    */
    inline int AddAmount(int addAmount)
    {
        amount_ += (addAmount = std::max(0, std::min(maxAmount_ - amount_, addAmount)));
        return addAmount;
    }

    /**
    * Returns the amount of items actually removed
    */
    inline int RemoveAmount(int removeAmount)
    {
        amount_ -= (removeAmount = std::max(0, std::min(amount_, removeAmount)));
        return removeAmount;
    }

    /**
    * Returns the actual amount of items set to
    */
    inline int SetAmount(int amount) { return amount_ = std::max(0, std::min(maxAmount_, amount)); }
    inline int GetAmount() const { return amount_; }

    /**
    * Returns the actual max amount set to
    */
    inline int SetMaxAmount(int maxAmount) { return maxAmount_ = std::max(0, maxAmount); }
    inline int GetMaxAmount() const { return maxAmount_; }

    inline bool IsMaxStack() const { return amount_ >= maxAmount_; }

    inline virtual sf::Time GetUseDelay() const { return sf::seconds(0.25f); }

    void TickDelayTimeLeft();

    inline void SetUseDelayTimeLeft(const sf::Time& delayTimeLeft) { useDelayLeft_ = delayTimeLeft; }
    inline sf::Time GetUseDelayTimeLeft() const { return useDelayLeft_; }

    virtual void Use(PlayerEntity* player) = 0;

    inline virtual void UseWithDelay(PlayerEntity* player)
    {
        if (useDelayLeft_ <= sf::Time::Zero) {
            Use(player);
            useDelayLeft_ = GetUseDelay();
        }
    }

    virtual std::string GetUseText() const = 0;

    inline virtual sf::Sprite GetSprite() const { return sf::Sprite(); }

    virtual std::string GetItemName() const = 0;
    virtual ItemType GetItemType() const = 0;
};

/**
* Different artefact types
*/
enum class ArtefactType
{
    Appearance1,
    Appearance2,
    Appearance3,
    Appearance4,
    Appearance5
};

/**
* Artefact item class
*/
class ArtefactItem : public Item
{
    ArtefactType artefactType_;

public:
    ArtefactItem(ArtefactType artefactType);
    virtual ~ArtefactItem();

    inline virtual void Use(PlayerEntity* player) override { }
    inline std::string GetUseText() const override { return std::string(); }

    sf::Sprite GetSprite() const override;

    inline std::string GetItemName() const override { return "Artefact Piece"; }
    inline virtual ItemType GetItemType() const override { return ItemType::ArtefactPiece; }

    inline ArtefactType GetArtefactType() const { return artefactType_; }
};


/**
* Potion item class
*/
class PotionItem : public Item
{
    ItemType potionType_;

public:
    PotionItem(ItemType potionType, int amount = 0);
    virtual ~PotionItem();

    void Use(PlayerEntity* player) override;

    inline std::string GetUseText() const override
    {
        switch (potionType_) {
        case ItemType::HealthPotion:
            return "Drink Health Potion (Restores 250 Health)";

        case ItemType::MagicPotion:
            return "Drink Magic Potion (Restores 250 Mana)";

        default:
            return std::string();
        }
    }

    sf::Sprite GetSprite() const override;

    std::string GetItemName() const override;
    inline ItemType GetItemType() const { return potionType_; }
};

/**
* Base weapon class
*/
class BaseWeaponItem : public Item
{
    std::string itemName_;

public:
    BaseWeaponItem(const std::string& itemName = std::string());
    virtual ~BaseWeaponItem();

    inline virtual std::string GetUseText() const override { return "Attack using " + itemName_; }

    inline void SetItemName(const std::string& itemName) { itemName_ = itemName; }
    inline std::string GetItemName() const override { return itemName_; }
};

/**
* Melee wep type
*/
enum class MeleeWeaponType
{
    BasicSword
};

/**
* Melee weapons
*/
class MeleeWeapon : public BaseWeaponItem
{
    MeleeWeaponType meleeWeaponType_;

public:
    MeleeWeapon(MeleeWeaponType meleeWeaponType);
    virtual ~MeleeWeapon();

    virtual void Use(PlayerEntity* player) override;

    sf::Sprite GetSprite() const override;

    inline ItemType GetItemType() const override { return ItemType::MeleeWeapon; }
    inline MeleeWeaponType GetMeleeWeaponType() const { return meleeWeaponType_; }
};

/**
* Magic wep type
*/
enum class MagicWeaponType
{
    ZeroStaff
};

/**
* Magic weapons
*/
class MagicWeapon : public BaseWeaponItem
{
    MagicWeaponType magicWeaponType_;

public:
    MagicWeapon(MagicWeaponType magicWeaponType);
    virtual ~MagicWeapon();

    virtual void Use(PlayerEntity* player) override;

    sf::Sprite GetSprite() const override;

    inline ItemType GetItemType() const override { return ItemType::MagicWeapon; }
    inline MagicWeaponType GetMagicWeaponType() const { return magicWeaponType_; }
};


/**
* Item entity
*/
class ItemEntity : public UnitEntity, public IPlayerUsable
{
    std::unique_ptr<Item> item_;

public:
    ItemEntity();
    virtual ~ItemEntity();

    virtual void Render(sf::RenderTarget& target) override;

    virtual void Use(EntityId playerId) override;

    virtual bool IsUsable(EntityId playerId) const override;

    inline virtual std::string GetUseText() const override
    {
        if (item_ && item_->GetAmount() > 0) {
            std::string useText = "Pick up " + item_->GetItemName();

            if (item_->GetAmount() > 1) {
                useText += " x " + std::to_string(item_->GetAmount());
            }

            return useText;
        }
        else {
            return std::string();
        }
    }

    inline void SetItem(std::unique_ptr<Item>& item) { item_ = std::move(item); }

    inline Item* GetItem() { return item_.get(); }
    inline const Item* GetItem() const { return item_.get(); }

    inline virtual std::string GetUnitName() const override { return item_ ? item_->GetItemName() : "Item"; }

    inline virtual std::string GetName() const override { return "ItemEntity"; }
};
