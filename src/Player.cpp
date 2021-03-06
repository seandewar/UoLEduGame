#include "Player.h"

#include <SFML/Window/Keyboard.hpp>
#include <SFML/Graphics/Sprite.hpp>
#include <SFML/Graphics/RectangleShape.hpp> // TODO Probably for debug

#include "World.h"
#include "Game.h"
#include "Helper.h"
#include "PlayerUsable.h"
#include "Stairs.h"


PlayerDefaultStartEntity::PlayerDefaultStartEntity()
{
}


PlayerDefaultStartEntity::~PlayerDefaultStartEntity()
{
}


PlayerInventory::PlayerInventory()
{
    ResetInventory();
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
    else if (item->GetItemType() == ItemType::Armour &&
        (!armour_ || armour_->GetAmount() <= 0)) {
        armour_ = std::make_unique<Armour>(static_cast<Armour&>(*item));
        receivedAmount = armour_->GetAmount();
        item->SetAmount(0);
    }

    // TODO specials ?

    if (receivedAmount > 0) {
        Game::Get().AddMessage("You received " + item->GetItemName() + " x " + std::to_string(receivedAmount));
        GameAssets::Get().pickupSound.play();
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
nextDir_(PlayerFacingDirection::Down),
dir_(PlayerFacingDirection::Down),
attackAnimTimeLeft_(sf::Time::Zero),
useTarget_(false),
handledDeath_(false)
{
    SetSize(sf::Vector2f(12.0f, 12.0f));
    InitAnimations();
}


PlayerEntity::~PlayerEntity()
{
}


void PlayerEntity::UseInventorySlot(PlayerInventorySlot slot)
{
    if (!inv_ || (GetStats() && !GetStats()->IsAlive())) {
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

    // TODO specials (weps & armour will drop automatically to accommodate)
    return true;
}


bool PlayerEntity::PickupItem(Item* item)
{
    auto area = GetAssignedArea();

    if (!item || item->GetAmount() <= 0 || !inv_ || !CanPickupItem(item) || !area) {
        return false;
    }

    if (item->GetItemType() == ItemType::MeleeWeapon ||
        item->GetItemType() == ItemType::MagicWeapon ||
        item->GetItemType() == ItemType::Armour) {
        auto itemEnt = area->GetEntity<ItemEntity>(area->EmplaceEntity<ItemEntity>());
        itemEnt->SetCenterPosition(GetCenterPosition());
        
        Item* droppedItem = nullptr;

        switch (item->GetItemType()) {
        case ItemType::MeleeWeapon:
            droppedItem = inv_->meleeWeapon_.release();
            break;

        case ItemType::MagicWeapon:
            droppedItem = inv_->magicWeapon_.release();
            break;

        case ItemType::Armour:
            droppedItem = inv_->armour_.release();
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
    else if (item->GetItemType() == ItemType::ArtefactPiece) {
        // artefact piece collected!
        Game::Get().AddMessage("Congratulations - you've found an artefact piece!");
        Game::Get().GetDirector().FoundArtefact(GetAssignedArea());
        item->SetAmount(0);
        GameAssets::Get().artefactPickupSound.play();
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

    deadAnim_.AddFrame(sf::Sprite(GameAssets::Get().playerSpriteSheet, sf::IntRect(0, 48, 16, 16)), sf::seconds(0.25f));
    deadAnim_.AddFrame(sf::Sprite(GameAssets::Get().playerSpriteSheet, sf::IntRect(16, 48, 16, 16)), sf::seconds(0.25f));
    deadAnim_.AddFrame(sf::Sprite(GameAssets::Get().playerSpriteSheet, sf::IntRect(32, 48, 16, 16)), sf::seconds(0.25f));
    deadAnim_.AddFrame(sf::Sprite(GameAssets::Get().playerSpriteSheet, sf::IntRect(48, 48, 16, 16)), sf::seconds(0.5f));
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


void PlayerEntity::TickMoveAnimations()
{
    upAnim_.Tick();
    downAnim_.Tick();
    leftAnim_.Tick();
    rightAnim_.Tick();
}


void PlayerEntity::AddMoveInDirection(PlayerFacingDirection dir)
{
    nextDir_ = dir;

    switch (dir) {
    case PlayerFacingDirection::Up:
        moveDir_.y -= 1.0f;
        break;

    case PlayerFacingDirection::Down:
        moveDir_.y += 1.0f;
        break;

    case PlayerFacingDirection::Left:
        moveDir_.x -= 1.0f;
        break;

    case PlayerFacingDirection::Right:
        moveDir_.x += 1.0f;
        break;
    }
}


void PlayerEntity::HandleMovement()
{
    if (!GetAssignedArea() || !GetStats()) {
        return;
    }

    if (dir_ != nextDir_ || (moveDir_.x == 0.0f && moveDir_.y == 0.0f)) {
        RestartAnimations();
    }
    else {
        Game::Get().ResetDisplayedQuestion(); // reset question interface if we moved and it's on
        TickMoveAnimations();
    }

    MoveWithCollision(moveDir_ * GetStats()->GetMoveSpeed() * Game::FrameTimeStep.asSeconds());
    dir_ = nextDir_;
    moveDir_ = sf::Vector2f();
}


void PlayerEntity::HandleUseNearbyObjects()
{
    auto area = GetAssignedArea();

    if (!area || Game::Get().GetDisplayedQuestion()) {
        useTarget_ = false;
        return;
    }

    // use target from last tick if use key was pressed
    if (useTarget_ && targettedUsableEnt_ != InvalidId) {
        auto usableEnt = dynamic_cast<PlayerUsable*>(area->GetEntity(targettedUsableEnt_));

        if (usableEnt && usableEnt->IsUsable(GetAssignedId())) {
            usableEnt->Use(GetAssignedId());
        }
    }

    // update target
    auto entsInRange = area->GetWorldEntitiesInRange(GetCenterPosition(), useRange_);

    EntityId closestUsableEntId = InvalidId;
    bool closestUsableEntIsCurrentlyUsable = false;
    float closestUsableEntDistSq = useRange_ * useRange_ + 1.0f;

    for (auto& eInfo : entsInRange) {
        auto usableEnt = dynamic_cast<PlayerUsable*>(area->GetEntity<WorldEntity>(eInfo.first));

        if (usableEnt) {
            bool currentlyUsable = usableEnt->IsUsable(GetAssignedId());
            bool isCloser = eInfo.second < closestUsableEntDistSq;

            // prioritise PlayerUsables that are currently usable over
            // those that are currently unusable within the proximity of the player
            if ((!closestUsableEntIsCurrentlyUsable && isCloser) ||
                (!closestUsableEntIsCurrentlyUsable && currentlyUsable) ||
                (closestUsableEntIsCurrentlyUsable && currentlyUsable && isCloser)) {
                closestUsableEntId = eInfo.first;
                closestUsableEntDistSq = eInfo.second;
                closestUsableEntIsCurrentlyUsable = currentlyUsable;
            }
        }
    }

    targettedUsableEnt_ = closestUsableEntId;
    useTarget_ = false;
}


void PlayerEntity::TickAttackAnimation()
{
    if (attackAnimTimeLeft_ > sf::Time::Zero) {
        attackAnimTimeLeft_ -= Game::FrameTimeStep;
    }
}


u32 PlayerEntity::Attack(u32 initialDamage, DamageType source)
{
    u32 damageReduction = 0;

    if (inv_ && inv_->GetArmour() && inv_->GetArmour()->GetAmount() > 0) {
        if (source == DamageType::Melee) {
            damageReduction = Helper::GenerateRandomInt<u32>(0, inv_->GetArmour()->GetMeleeDefense());
        }
        else if (source == DamageType::Magic) {
            damageReduction = Helper::GenerateRandomInt<u32>(0, inv_->GetArmour()->GetMagicDefense());
        }
    }

    return AliveEntity::Attack(static_cast<u32>(std::max<i64>(0, static_cast<i64>(initialDamage)-damageReduction)),
        source);
}


u32 PlayerEntity::DamageWithoutInvincibility(u32 amount, DamageType source)
{
    Game::Get().ResetDisplayedQuestion(); // interrupt question interface if it's up
    Game::Get().NotifyPlayerDamaged(amount);

    GameAssets::Get().playerHurtSound.play();
    return AliveEntity::Damage(amount, source);
}


void PlayerEntity::Tick()
{
    if (GetStats() && !GetStats()->IsAlive()) {
        // player is dead
        useTarget_ = false;
        targettedUsableEnt_ = InvalidId;
        moveDir_ = sf::Vector2f();
        invincibilityTime_ = sf::Time::Zero;
        attackAnimTimeLeft_ = sf::Time::Zero;
        dir_ = PlayerFacingDirection::Down;

        // handle death specific stuff for this
        // death
        if (!handledDeath_) {
            GameAssets::Get().playerDeathSound.play();
            Game::Get().AddMessage("Oh dear - you have been knocked out!", sf::Color(255, 0, 0));

            Game::Get().NotifyPlayerDeath();
            Game::Get().GetDirector().PlayerKilled(GetAssignedArea());
            handledDeath_ = true;
        }

        deadAnim_.Tick();
    }
    else {
        // player is alive
        handledDeath_ = false;

        HandleMovement();
        HandleUseNearbyObjects();
        TickAttackAnimation();

        if (inv_) {
            inv_->TickUseDelays();
        }

        if (invincibilityTime_ > sf::Time::Zero) {
            invincibilityTime_ -= Game::FrameTimeStep;
        }
    }
}


void PlayerEntity::Render(sf::RenderTarget& target)
{
    bool isDead = GetStats() && !GetStats()->IsAlive();

    // player sprite
    sf::Sprite playerSprite;

    if (!isDead) {
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
    }
    else {
        // display dead sprite
        playerSprite = deadAnim_.GetCurrentFrame();
    }

    playerSprite.setPosition(GetCenterPosition() - sf::Vector2f(8.0f, 8.0f));

    // armour sprite
    sf::Sprite armourSprite;

    // check if we have armour to render
    if (inv_ && inv_->GetArmour() && inv_->GetArmour()->GetAmount() > 0) {
        // if dead, render armour as item
        if (!isDead) {
            armourSprite = inv_->GetArmour()->GetPlayerSprite(dir_);
        }
        else {
            armourSprite = inv_->GetArmour()->GetSprite();
        }
    }

    if (!isDead) {
        // render armour to match head pos
        armourSprite.setPosition(playerSprite.getPosition());
    }
    else {
        // armour meeds to look like its been dropped in death
        armourSprite.setPosition(playerSprite.getPosition() + sf::Vector2f(-12.5f, 10.0f));
    }

    if (!isDead) {
        // invincibility effect
        if (HasInvincibility()) {
            sf::Color invincColor(255, 255, 255, Helper::GenerateRandomInt(0, 155));
            playerSprite.setColor(invincColor);
            armourSprite.setColor(invincColor);
        }
        else {
            playerSprite.setColor(sf::Color(255, 255, 255, 255));
            armourSprite.setColor(sf::Color(255, 255, 255, 255));
        }

        // render weapon if attacking
        sf::Sprite weaponSprite;

        if (inv_ && attackAnimTimeLeft_ > sf::Time::Zero) {
            auto weapon = inv_->GetWeaponItem(attackAnimWeapon_);

            if (weapon && weapon->GetAmount() > 0) {
                weaponSprite = weapon->GetSprite();

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
            target.draw(armourSprite);
            target.draw(weaponSprite);
        }
        else {
            target.draw(weaponSprite);
            target.draw(playerSprite);
            target.draw(armourSprite);
        }
    }
    else {
        // if we're dead, just render the armour and player sprite as
        // if the player was facing downwards
        target.draw(playerSprite);
        target.draw(armourSprite);
    }
}
