#include "Item.h"

#include "Game.h"


Item::Item()
{
}


Item::~Item()
{
}


PotionItem::PotionItem(PotionItemType potionType) :
Item(),
potionType_(potionType)
{
}


PotionItem::~PotionItem()
{
}


sf::Sprite PotionItem::GetSprite() const
{
    sf::Sprite potionSprite(GameAssets::Get().itemsSpriteSheet);

    switch (potionType_) {
    case PotionItemType::Health:
        potionSprite.setTextureRect(sf::IntRect(0, 0, 16, 16));
        break;

    case PotionItemType::Magic:
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
    case PotionItemType::Health:
        return "Health Potion";

    case PotionItemType::Magic:
        return "Magic Potion";

    default:
        return "Potion";
    }
}


ItemContainer::ItemContainer() :
maxItems_(1000)
{
}


ItemContainer::ItemContainer(ItemContainer&& other) :
maxItems_(other.maxItems_),
items_(std::move(other.maxItems_))
{
}


ItemContainer::~ItemContainer()
{
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
    if (item_) {
        auto itemSprite = item_->GetSprite();
        itemSprite.setPosition(GetPosition());

        target.draw(itemSprite);
    }
}


void ItemEntity::Use(EntityId playerId)
{
    if (item_) {
        auto playerEnt = GetAssignedArea()->GetEntity<PlayerEntity>(playerId);
        
        if (playerEnt && playerEnt->GetItems()) {
            playerEnt->GetItems()->AddItem(item_);
        }

        MarkForDeletion();
    }
}