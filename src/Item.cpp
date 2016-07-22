#include "Item.h"

#include "Game.h"


Item::Item(int maxAmount, int amount)
{
    SetMaxAmount(maxAmount);
    SetAmount(amount);
}


Item::~Item()
{
}


PotionItem::PotionItem(ItemType potionType, int amount) :
Item(10, amount),
potionType_(potionType)
{
    if (potionType_ != ItemType::HealthPotion &&
        potionType != ItemType::MagicPotion) {
        throw std::runtime_error("Bad potion ItemType!");
    }
}


PotionItem::~PotionItem()
{
}


sf::Sprite PotionItem::GetSprite() const
{
    sf::Sprite potionSprite(GameAssets::Get().itemsSpriteSheet);

    switch (potionType_) {
    case ItemType::HealthPotion:
        potionSprite.setTextureRect(sf::IntRect(0, 0, 16, 16));
        break;

    case ItemType::MagicPotion:
        potionSprite.setTextureRect(sf::IntRect(0, 16, 16, 16));
        break;

    default:
        return sf::Sprite();
    }

    return potionSprite;
}


std::string PotionItem::GetItemName() const
{
    switch (potionType_) {
    case ItemType::HealthPotion:
        return "Health Potion";

    case ItemType::MagicPotion:
        return "Magic Potion";

    default:
        return "Potion";
    }
}


ItemEntity::ItemEntity() :
UnitEntity()
{
    SetSize(sf::Vector2f(16.0f, 16.0f));
}


ItemEntity::~ItemEntity()
{
}


void ItemEntity::Render(sf::RenderTarget& target)
{
    if (item_ && item_->GetAmount() > 0) {
        auto itemSprite = item_->GetSprite();
        itemSprite.setPosition(GetPosition());

        target.draw(itemSprite);
    }
}


void ItemEntity::Use(EntityId playerId)
{
    if (item_) {
        auto playerEnt = GetAssignedArea()->GetEntity<PlayerEntity>(playerId);
        
        if (playerEnt && playerEnt->GetInventory()) {
            playerEnt->GetInventory()->GiveItem(item_.get());
        }

        if (item_->GetAmount() <= 0) {
            MarkForDeletion();
        }
    }
}


bool ItemEntity::IsUsable(EntityId playerId) const
{
    auto area = GetAssignedArea();
    if (!area) {
        return false;
    }

    auto playerEnt = area->GetEntity<PlayerEntity>(playerId);
    if (!playerEnt || !playerEnt->GetInventory()) {
        return false;
    }

    return item_ && item_->GetAmount() > 0 && playerEnt->GetInventory()->CanGiveItem(item_.get());
}