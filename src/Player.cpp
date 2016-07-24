#include "Player.h"

#include <SFML/Window/Keyboard.hpp>
#include <SFML/Graphics/Sprite.hpp>
#include <SFML/Graphics/RectangleShape.hpp> // TODO Probably for debug

#include "World.h"
#include "Game.h"
#include "Helper.h"
#include "IPlayerUsable.h"
#include "Stairs.h"


PlayerDefaultStartEntity::PlayerDefaultStartEntity()
{
}


PlayerDefaultStartEntity::~PlayerDefaultStartEntity()
{
}


PlayerInventory::PlayerInventory() :
selectedWeapon_(PlayerSelectedWeapon::Melee),
meleeWeapon_(std::make_unique<MeleeWeapon>(MeleeWeaponType::BasicSword)),
magicWeapon_(std::make_unique<MagicWeapon>(MagicWeaponType::ZeroStaff)),
healthPotions_(std::make_unique<PotionItem>(ItemType::HealthPotion)),
magicPotions_(std::make_unique<PotionItem>(ItemType::MagicPotion))
{
}


PlayerInventory::~PlayerInventory()
{
}


void PlayerInventory::TickUseDelays()
{
    if (meleeWeapon_) {
        meleeWeapon_->TickDelayTimeLeft();
    }
    if (magicWeapon_) {
        magicWeapon_->TickDelayTimeLeft();
    }
    if (healthPotions_) {
        healthPotions_->TickDelayTimeLeft();
    }
    if (magicPotions_) {
        magicPotions_->TickDelayTimeLeft();
    }
}


void PlayerInventory::GiveItem(Item* item)
{
    if (!item || item->GetAmount() <= 0) {
        return;
    }

    int receivedAmount = 0;

    if (item->GetItemType() == ItemType::HealthPotion) {
        item->RemoveAmount(receivedAmount = healthPotions_->AddAmount(item->GetAmount()));
    }
    else if (item->GetItemType() == ItemType::MagicPotion) {
        item->RemoveAmount(receivedAmount = magicPotions_->AddAmount(item->GetAmount()));
    }
    else if (item->GetItemType() == ItemType::MeleeWeapon &&
        (!meleeWeapon_ || meleeWeapon_->GetAmount() <= 0)) {
        meleeWeapon_ = std::make_unique<MeleeWeapon>(static_cast<MeleeWeapon&>(*item));
        receivedAmount = meleeWeapon_->GetAmount();
        item->SetAmount(0);
    }
    else if (item->GetItemType() == ItemType::MagicWeapon &&
        (!magicWeapon_ || magicWeapon_->GetAmount() <= 0)) {
        magicWeapon_ = std::make_unique<MagicWeapon>(static_cast<MagicWeapon&>(*item));
        receivedAmount = magicWeapon_->GetAmount();
        item->SetAmount(0);
    }

    // TODO specials

    if (receivedAmount > 0) {
        Game::Get().AddMessage("You received " + item->GetItemName() + " x " + std::to_string(receivedAmount));
    }
    else if (receivedAmount == 0) {
        Game::Get().AddMessage("You cannot carry another " + item->GetItemName() + ".");
    }
}


const sf::Time PlayerEntity::attackAnimDuration_ = sf::seconds(0.2f);


PlayerEntity::PlayerEntity() :
AliveEntity(),
useRange_(25.0f),
targettedUsableEnt_(InvalidId),
inv_(nullptr),
dir_(PlayerFacingDirection::Down),
attackAnimTimeLeft_(sf::Time::Zero)
{
    SetSize(sf::Vector2f(12.0f, 12.0f));
    InitAnimations();
}


PlayerEntity::~PlayerEntity()
{
}


void PlayerEntity::UseInventorySlot(PlayerInventorySlot slot)
{
    if (!inv_) {
        return;
    }

    // find out what item we want to use
    Item* itemToUse = nullptr;

    switch (slot) {
    case PlayerInventorySlot::MeleeWeapon:
        inv_->SetSelectedWeapon(PlayerSelectedWeapon::Melee);
        itemToUse = inv_->GetMeleeWeapon();
        break;

    case PlayerInventorySlot::MagicWeapon:
        inv_->SetSelectedWeapon(PlayerSelectedWeapon::Magic);
        itemToUse = inv_->GetMagicWeapon();
        break;

    case PlayerInventorySlot::HealthPotions:
        itemToUse = inv_->GetHealthPotions();
        break;

    case PlayerInventorySlot::MagicPotions:
        itemToUse = inv_->GetMagicPotions();
        break;

    default:
        return;
    }

    // use chosen item
    if (itemToUse && itemToUse->GetAmount() > 0) {
        itemToUse->UseWithDelay(this);
    }
}


bool PlayerEntity::CanPickupItem(Item* item) const
{
    if (!inv_ || !item || item->GetAmount() <= 0) {
        return false;
    }

    if (item->GetItemType() == ItemType::HealthPotion && inv_->GetHealthPotions()->IsMaxStack()) {
        return false;
    }
    else if (item->GetItemType() == ItemType::MagicPotion && inv_->GetMagicPotions()->IsMaxStack()) {
        return false;
    }

    // TODO specials (weps will drop automatically to accommodate)
    return true;
}


bool PlayerEntity::PickupItem(Item* item)
{
    auto area = GetAssignedArea();

    if (!item || item->GetAmount() <= 0 || !inv_ || !CanPickupItem(item) || !area) {
        return false;
    }

    if (item->GetItemType() == ItemType::MeleeWeapon ||
        item->GetItemType() == ItemType::MagicWeapon) {
        auto itemEnt = area->GetEntity<ItemEntity>(area->EmplaceEntity<ItemEntity>());
        itemEnt->SetCenterPosition(GetCenterPosition());
        
        BaseWeaponItem* droppedItem = nullptr;

        switch (item->GetItemType()) {
        case ItemType::MeleeWeapon:
            droppedItem = inv_->meleeWeapon_.release();
            break;

        case ItemType::MagicWeapon:
            droppedItem = inv_->magicWeapon_.release();
            break;
        }

        if (droppedItem && droppedItem->GetAmount() > 0) {
            if (droppedItem->GetAmount() > 1) {
                Game::Get().AddMessage("You drop your " + droppedItem->GetItemName() + " x " +
                    std::to_string(droppedItem->GetAmount()));
            }
            else {
                Game::Get().AddMessage("You drop your " + droppedItem->GetItemName() + ".");
            }

            itemEnt->SetItem(std::unique_ptr<Item>(droppedItem));
        }
    }

    inv_->GiveItem(item);
    return true;
}


void PlayerEntity::InitAnimations()
{
    upAnim_.AddFrame(sf::Sprite(GameAssets::Get().playerSpriteSheet, sf::IntRect(16, 0, 16, 16)));
    upAnim_.AddFrame(sf::Sprite(GameAssets::Get().playerSpriteSheet, sf::IntRect(16, 16, 16, 16)));
    upAnim_.AddFrame(sf::Sprite(GameAssets::Get().playerSpriteSheet, sf::IntRect(16, 0, 16, 16)));
    upAnim_.AddFrame(sf::Sprite(GameAssets::Get().playerSpriteSheet, sf::IntRect(16, 32, 16, 16)));

    downAnim_.AddFrame(sf::Sprite(GameAssets::Get().playerSpriteSheet, sf::IntRect(0, 0, 16, 16)));
    downAnim_.AddFrame(sf::Sprite(GameAssets::Get().playerSpriteSheet, sf::IntRect(0, 16, 16, 16)));
    downAnim_.AddFrame(sf::Sprite(GameAssets::Get().playerSpriteSheet, sf::IntRect(0, 0, 16, 16)));
    downAnim_.AddFrame(sf::Sprite(GameAssets::Get().playerSpriteSheet, sf::IntRect(0, 32, 16, 16)));

    leftAnim_.AddFrame(sf::Sprite(GameAssets::Get().playerSpriteSheet, sf::IntRect(32, 0, 16, 16)));
    leftAnim_.AddFrame(sf::Sprite(GameAssets::Get().playerSpriteSheet, sf::IntRect(32, 16, 16, 16)));
    leftAnim_.AddFrame(sf::Sprite(GameAssets::Get().playerSpriteSheet, sf::IntRect(32, 0, 16, 16)));
    leftAnim_.AddFrame(sf::Sprite(GameAssets::Get().playerSpriteSheet, sf::IntRect(32, 32, 16, 16)));

    rightAnim_.AddFrame(sf::Sprite(GameAssets::Get().playerSpriteSheet, sf::IntRect(48, 0, 16, 16)));
    rightAnim_.AddFrame(sf::Sprite(GameAssets::Get().playerSpriteSheet, sf::IntRect(48, 16, 16, 16)));
    rightAnim_.AddFrame(sf::Sprite(GameAssets::Get().playerSpriteSheet, sf::IntRect(48, 0, 16, 16)));
    rightAnim_.AddFrame(sf::Sprite(GameAssets::Get().playerSpriteSheet, sf::IntRect(48, 32, 16, 16)));
}


bool PlayerEntity::SetPositionToDefaultStart()
{
    auto area = GetAssignedArea();

    if (!area) {
        return false;
    }

    auto startEnts = area->GetAllEntitiesOfType<PlayerDefaultStartEntity>();
    if (startEnts.size() <= 0) {
        return false;
    }

    auto chosenStartEnt = area->GetEntity<PlayerDefaultStartEntity>(
        startEnts[Helper::GenerateRandomInt<std::size_t>(0, startEnts.size() - 1)]);

    if (!chosenStartEnt) {
        return false;
    }

    SetPosition(chosenStartEnt->GetPosition());
    return true;
}


void PlayerEntity::RestartAnimations()
{
    upAnim_.Restart();
    downAnim_.Restart();
    leftAnim_.Restart();
    rightAnim_.Restart();
}


void PlayerEntity::TickAnimations()
{
    upAnim_.Tick();
    downAnim_.Tick();
    leftAnim_.Tick();
    rightAnim_.Tick();
}


void PlayerEntity::HandleMovement()
{
    if (!GetAssignedArea() || !GetStats()) {
        return;
    }

    sf::Vector2f moveDir;
    PlayerFacingDirection faceDir = dir_;

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::W) || sf::Keyboard::isKeyPressed(sf::Keyboard::Up)) {
        moveDir.y -= 1.0f;
        faceDir = PlayerFacingDirection::Up;
    }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::S) || sf::Keyboard::isKeyPressed(sf::Keyboard::Down)) {
        moveDir.y += 1.0f;
        faceDir = PlayerFacingDirection::Down;
    }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::A) || sf::Keyboard::isKeyPressed(sf::Keyboard::Left)) {
        moveDir.x -= 1.0f;
        faceDir = PlayerFacingDirection::Left;
    }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::D) || sf::Keyboard::isKeyPressed(sf::Keyboard::Right)) {
        moveDir.x += 1.0f;
        faceDir = PlayerFacingDirection::Right;
    }

    if (dir_ != faceDir || moveDir.x == 0.0f && moveDir.y == 0.0f) {
        RestartAnimations();
    }
    else {
        TickAnimations();
    }

    dir_ = faceDir;

    MoveWithCollision(moveDir * GetStats()->GetMoveSpeed() * Game::FrameTimeStep.asSeconds());
}


void PlayerEntity::HandleUseNearbyObjects()
{
    auto area = GetAssignedArea();

    if (!area) {
        return;
    }

    // use target from last tick if use key was pressed
    if (Game::Get().IsKeyPressedFromEvent(sf::Keyboard::E) && targettedUsableEnt_ != InvalidId) {
        auto usableEnt = dynamic_cast<IPlayerUsable*>(area->GetEntity(targettedUsableEnt_));

        if (usableEnt && usableEnt->IsUsable(GetAssignedId())) {
            usableEnt->Use(GetAssignedId());
        }
    }

    // update target
    auto entsInRange = area->GetWorldEntitiesInRange(GetCenterPosition(), useRange_);

    EntityId closestUsableEntId = InvalidId;
    float closestUsableEntDistSq = useRange_ * useRange_ + 1.0f;

    for (auto& eInfo : entsInRange) {
        auto usableEnt = dynamic_cast<IPlayerUsable*>(area->GetEntity<WorldEntity>(eInfo.first));

        if (usableEnt && usableEnt->IsUsable(GetAssignedId()) && eInfo.second < closestUsableEntDistSq) {
            closestUsableEntId = eInfo.first;
            closestUsableEntDistSq = eInfo.second;
        }
    }

    targettedUsableEnt_ = closestUsableEntId;
}


void PlayerEntity::TickAttackAnimation()
{
    if (attackAnimTimeLeft_ > sf::Time::Zero) {
        attackAnimTimeLeft_ -= Game::FrameTimeStep;
    }
}


void PlayerEntity::Tick()
{
    HandleMovement();
    HandleUseNearbyObjects();
    TickAttackAnimation();

    if (inv_) {
        inv_->TickUseDelays();
    }
}


void PlayerEntity::Render(sf::RenderTarget& target)
{
    // player sprite
    sf::Sprite playerSprite;

    switch (dir_) {
    case PlayerFacingDirection::Up:
        playerSprite = upAnim_.GetCurrentFrame();
        break;

    case PlayerFacingDirection::Down:
        playerSprite = downAnim_.GetCurrentFrame();
        break;

    case PlayerFacingDirection::Left:
        playerSprite = leftAnim_.GetCurrentFrame();
        break;

    case PlayerFacingDirection::Right:
        playerSprite = rightAnim_.GetCurrentFrame();
        break;
    }

    playerSprite.setPosition(GetCenterPosition() - sf::Vector2f(8.0f, 8.0f));

    // render weapon if attacking
    sf::Sprite weaponSprite;

    if (inv_ && attackAnimTimeLeft_ > sf::Time::Zero) {
        auto weapon = inv_->GetWeaponItem(attackAnimWeapon_);

        if (weapon && weapon->GetAmount() > 0) {
            weaponSprite = weapon->GetSprite();
            float animTimeFactor = 1.0f - (attackAnimTimeLeft_ / attackAnimDuration_);

            switch (dir_) {
            case PlayerFacingDirection::Up:
                weaponSprite.setPosition(GetPosition() + sf::Vector2f(-6.0f, -8.0f));
                break;

            case PlayerFacingDirection::Right:
                weaponSprite.setRotation(90.0f);
                weaponSprite.setPosition(GetPosition() + sf::Vector2f(22.0f, 0.0f));
                break;

            case PlayerFacingDirection::Down:
                weaponSprite.setRotation(180.0f);
                weaponSprite.setPosition(GetPosition() + sf::Vector2f(18.0f, 23.0f));
                break;

            case PlayerFacingDirection::Left:
                weaponSprite.setRotation(-90.0f);
                weaponSprite.setPosition(GetPosition() + sf::Vector2f(-10.0f, 16.0f));
                break;
            }
        }
    }

    if (dir_ == PlayerFacingDirection::Down || dir_ == PlayerFacingDirection::Left) {
        target.draw(playerSprite);
        target.draw(weaponSprite);
    }
    else {
        target.draw(weaponSprite);
        target.draw(playerSprite);
    }
}
