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
    Potion,
    Weapon
};

/**
* Base item class
*/
class Item
{
public:
    Item();
    virtual ~Item();

    inline virtual sf::Sprite GetSprite() const { return sf::Sprite(); }

    virtual std::string GetItemName() const = 0;
    virtual ItemType GetItemType() const = 0;
};

/**
* The type of potion
*/
enum class PotionItemType
{
    Health,
    Magic
};

/**
* Potion item class
*/
class PotionItem : public Item
{
    PotionItemType potionType_;

public:
    PotionItem(PotionItemType potionType);
    virtual ~PotionItem();

    sf::Sprite GetSprite() const override;

    std::string GetItemName() const override;
    inline ItemType GetItemType() const { return ItemType::Potion; }

    inline PotionItemType GetPotionType() const { return potionType_; }
};

/**
* Class for containing items.
*/
class ItemContainer
{
    std::vector<std::unique_ptr<Item>> items_;
    std::size_t maxItems_;

public:
    ItemContainer();
    ItemContainer(ItemContainer&& other);
    ~ItemContainer();

    inline ItemContainer& operator=(ItemContainer&& other)
    {
        maxItems_ = other.maxItems_;
        items_ = std::move(other.items_);
        return *this;
    }

    template <typename T = Item>
    inline bool AddItem(std::unique_ptr<T>& item)
    {
        if (items_.size() >= maxItems_) {
            return false;
        }

        items_.emplace_back(std::move(item));
        return true;
    }

    inline void SetMaxItems(std::size_t maxItems) { maxItems_ = maxItems; }
    inline std::size_t GetMaxItems() const { return maxItems_; }

    inline std::vector<std::unique_ptr<Item>>& GetItems() { return items_; }
    inline const std::vector<std::unique_ptr<Item>>& GetItems() const { return items_; }

    inline std::size_t GetItemCount() const { return items_.size(); }
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

    inline virtual bool IsUsable() const override { return item_ != nullptr; }

    inline virtual std::string GetUseText() const override
    {
        return item_ ? "Pick up " + item_->GetItemName() : std::string();
    }

    inline void SetItem(std::unique_ptr<Item>& item) { item_ = std::move(item); }

    inline Item* GetItem() { return item_.get(); }
    inline const Item* GetItem() const { return item_.get(); }

    inline virtual std::string GetUnitName() const override { return item_ ? item_->GetItemName() : "Item"; }

    inline virtual std::string GetName() const override { return "ItemEntity"; }
};
