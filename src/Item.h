#pragma once

#include <string>
#include <memory>

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
    Weapon
};

/**
* Base item class
*/
class Item
{
    int amount_, maxAmount_;

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

    inline virtual sf::Sprite GetSprite() const { return sf::Sprite(); }

    virtual std::string GetItemName() const = 0;
    virtual ItemType GetItemType() const = 0;
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

    sf::Sprite GetSprite() const override;

    std::string GetItemName() const override;
    inline ItemType GetItemType() const { return potionType_; }
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
