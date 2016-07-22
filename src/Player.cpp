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
healthPotions_(std::make_unique<PotionItem>(ItemType::HealthPotion)),
magicPotions_(std::make_unique<PotionItem>(ItemType::MagicPotion))
{
}


PlayerInventory::~PlayerInventory()
{
}


bool PlayerInventory::CanGiveItem(Item* item) const
{
    if (!item || item->GetAmount() <= 0) {
        return false;
    }

    if (item->GetItemType() == ItemType::HealthPotion && healthPotions_->IsMaxStack()) {
        return false;
    }
    else if (item->GetItemType() == ItemType::MagicPotion && magicPotions_->IsMaxStack()) {
        return false;
    }

    // TODO specials (weps will drop automatically to accommodate)
    return true;
}


void PlayerInventory::GiveItem(Item* item)
{
    if (!item || item->GetAmount() <= 0) {
        return;
    }

    if (item->GetItemType() == ItemType::HealthPotion) {
        item->RemoveAmount(healthPotions_->AddAmount(item->GetAmount()));
    }
    else if (item->GetItemType() == ItemType::MagicPotion) {
        item->RemoveAmount(magicPotions_->AddAmount(item->GetAmount()));
    }

    // TODO weapons (melee & magic) & specials
}


PlayerEntity::PlayerEntity() :
AliveEntity(),
useRange_(25.0f),
targettedUsableEnt_(InvalidId),
inv_(nullptr),
dir_(PlayerFacingDirection::Down)
{
    SetSize(sf::Vector2f(12.0f, 12.0f));
    InitAnimations();
}


PlayerEntity::~PlayerEntity()
{
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


void PlayerEntity::Tick()
{
    HandleMovement();
    HandleUseNearbyObjects();
}


void PlayerEntity::Render(sf::RenderTarget& target)
{
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
    target.draw(playerSprite);
}
