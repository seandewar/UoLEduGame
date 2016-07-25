#include "Item.h"

#include "Game.h"
#include "Player.h"


Item::Item(int maxAmount, int amount)
{
    SetMaxAmount(maxAmount);
    SetAmount(amount);
}


Item::~Item()
{
}


void Item::TickDelayTimeLeft()
{
    if (useDelayLeft_ > sf::Time::Zero) {
        useDelayLeft_ -= Game::FrameTimeStep;
    }
}


ArtefactItem::ArtefactItem(ArtefactType artefactType) :
Item(1, 1),
artefactType_(artefactType)
{
}


ArtefactItem::~ArtefactItem()
{
}


sf::Sprite ArtefactItem::GetSprite() const
{
    sf::Sprite artefactSprite(GameAssets::Get().itemsSpriteSheet);

    switch (artefactType_) {
    case ArtefactType::Appearance1:
        artefactSprite.setTextureRect(sf::IntRect(0, 32, 16, 16));
        break;

    case ArtefactType::Appearance2:
        artefactSprite.setTextureRect(sf::IntRect(0, 48, 16, 16));
        break;

    case ArtefactType::Appearance3:
        artefactSprite.setTextureRect(sf::IntRect(0, 64, 16, 16));
        break;

    case ArtefactType::Appearance4:
        artefactSprite.setTextureRect(sf::IntRect(0, 80, 16, 16));
        break;

    case ArtefactType::Appearance5:
        artefactSprite.setTextureRect(sf::IntRect(0, 96, 16, 16));
        break;

    default:
        return sf::Sprite();
    }

    return artefactSprite;
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


void PotionItem::Use(PlayerEntity* player)
{
    if (!player || !player->GetStats() || GetAmount() <= 0) {
        return;
    }

    auto stats = player->GetStats();

    switch (potionType_) {
    case ItemType::HealthPotion:
        if (stats->GetHealth() < stats->GetMaxHealth()) {
            stats->ApplyHealing(250);
            RemoveAmount(1);

            Game::Get().AddMessage("You drink a Health Potion.", sf::Color(255, 100, 100));
        }
        else {
            Game::Get().AddMessage("You are already at full Health.");
        }
        break;

    case ItemType::MagicPotion:
        if (stats->GetMana() < stats->GetMaxMana()) {
            stats->SetMana(std::min(stats->GetMaxMana(), stats->GetMana() + 250));
            RemoveAmount(1);

            Game::Get().AddMessage("You drink a Magic Potion.", sf::Color(100, 100, 255));
        }
        else {
            Game::Get().AddMessage("You are already have full Mana.");
        }
        break;
    }
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


BaseWeaponItem::BaseWeaponItem(const std::string& itemName) :
Item(1, 1),
itemName_(itemName)
{
}


BaseWeaponItem::~BaseWeaponItem()
{
}


MeleeWeapon::MeleeWeapon(MeleeWeaponType meleeWeaponType) :
meleeWeaponType_(meleeWeaponType)
{
    if (meleeWeaponType_ == MeleeWeaponType::BasicSword) {
        SetItemName("Basic Sword");
    }
}


MeleeWeapon::~MeleeWeapon()
{
}


void MeleeWeapon::Use(PlayerEntity* player)
{
    if (!player || GetAmount() <= 0) {
        return;
    }

    // TODO
    player->PlayAttackAnimation(PlayerSelectedWeapon::Melee);
}


sf::Sprite MeleeWeapon::GetSprite() const
{
    sf::Sprite weaponSprite(GameAssets::Get().itemsSpriteSheet);

    switch (meleeWeaponType_) {
    case MeleeWeaponType::BasicSword:
        weaponSprite.setTextureRect(sf::IntRect(16, 0, 16, 16));
        break;

    default:
        return sf::Sprite();
    }

    return weaponSprite;
}


MagicWeapon::MagicWeapon(MagicWeaponType magicWeaponType) :
magicWeaponType_(magicWeaponType)
{
    if (magicWeaponType_ == MagicWeaponType::ZeroStaff) {
        SetItemName("Zero Staff");
    }
}


MagicWeapon::~MagicWeapon()
{
}


void MagicWeapon::Use(PlayerEntity* player)
{
    if (!player || GetAmount() <= 0) {
        return;
    }

    // TODO
    player->PlayAttackAnimation(PlayerSelectedWeapon::Magic);
}


sf::Sprite MagicWeapon::GetSprite() const
{
    sf::Sprite weaponSprite(GameAssets::Get().itemsSpriteSheet);

    switch (magicWeaponType_) {
    case MagicWeaponType::ZeroStaff:
        weaponSprite.setTextureRect(sf::IntRect(16, 16, 16, 16));
        break;

    default:
        return sf::Sprite();
    }

    return weaponSprite;
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
            playerEnt->PickupItem(item_.get());
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

    return item_ && item_->GetAmount() > 0 && playerEnt->CanPickupItem(item_.get());
}