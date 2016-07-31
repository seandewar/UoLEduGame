#include "Enemy.h"

#include <cassert>

#include <SFML/Graphics/RectangleShape.hpp>

#include "Game.h"
#include "Helper.h"


Enemy::Enemy() :
AliveEntity()
{
    SetSize(sf::Vector2f(16.0f, 16.0f));
}


Enemy::~Enemy()
{
}


void Enemy::Render(sf::RenderTarget& target)
{
    auto area = GetAssignedArea();

    if (!area) {
        return;
    }

    // render stats
    auto stats = GetStats();

    if (stats && stats->IsAlive()) {
        auto health = stats->GetHealth();
        auto maxHealth = stats->GetMaxHealth();

        if (maxHealth > 0) {
            // render health bar bg
            auto healthBarBg = std::make_unique<sf::RectangleShape>(sf::Vector2f(22.0f, 3.0f));
            healthBarBg->setFillColor(sf::Color(20, 20, 20));
            healthBarBg->setPosition(GetCenterPosition().x - 0.5f * healthBarBg->getSize().x, GetPosition().y - 4.0f);
            healthBarBg->setOutlineThickness(0.5f);
            healthBarBg->setOutlineColor(sf::Color(0, 0, 0));

            // render health bar fg
            auto healthBarXSizeMul = std::min(1.0f, std::max(0.0f, static_cast<float>(health) / maxHealth));

            auto healthBarFg = std::make_unique<sf::RectangleShape>(sf::Vector2f(
                healthBarBg->getSize().x * healthBarXSizeMul, healthBarBg->getSize().y));
            healthBarFg->setFillColor(sf::Color(255, 0, 0));
            healthBarFg->setPosition(healthBarBg->getPosition());

            // render health text label
            auto healthLabelText = std::make_unique<sf::Text>(GetUnitName(), GameAssets::Get().gameFont, 12);
            healthLabelText->setScale(0.15f, 0.2f);
            healthLabelText->setPosition(healthBarBg->getPosition() + 0.5f * healthBarBg->getSize() - 0.5f *
                sf::Vector2f(healthLabelText->getGlobalBounds().width, healthLabelText->getGlobalBounds().height));
            healthLabelText->setColor(sf::Color(255, 255, 255));

            auto healthLabelShadow = Helper::GetTextDropShadow(*healthLabelText, sf::Vector2f(0.2f, 0.2f));

            area->AddFrameUIRenderable(healthBarBg);
            area->AddFrameUIRenderable(healthBarFg);
            area->AddFrameUIRenderable(healthLabelShadow);
            area->AddFrameUIRenderable(healthLabelText);
        }
    }
}


BasicEnemy::BasicEnemy(EnemyType enemyType) :
Enemy(),
enemyType_(enemyType),
droppedItems_(false)
{
    if (enemyType_ != EnemyType::SkeletonBasic &&
        enemyType_ != EnemyType::GreenBlobBasic &&
        enemyType_ != EnemyType::BlueBlobBasic &&
        enemyType_ != EnemyType::RedBlobBasic &&
        enemyType_ != EnemyType::PinkBlobBasic &&
        enemyType_ != EnemyType::GhostBasic &&
        enemyType_ != EnemyType::MagicFlameBasic) {
        throw std::runtime_error("Bad BasicEnemy enemy type!");
    }

    // set hbox size
    switch (enemyType_) {
    case EnemyType::GreenBlobBasic:
    case EnemyType::BlueBlobBasic:
    case EnemyType::RedBlobBasic:
    case EnemyType::PinkBlobBasic:
        SetSize(sf::Vector2f(10.0f, 6.0f));
        break;

    case EnemyType::SkeletonBasic:
    case EnemyType::GhostBasic:
    case EnemyType::MagicFlameBasic:
        SetSize(sf::Vector2f(16.0f, 16.0f));
        break;
    }

    SetupAnimations();
    ResetStats();
}


BasicEnemy::~BasicEnemy()
{
}


void BasicEnemy::SetupAnimations()
{
    switch (enemyType_) {
    default:
        assert(!"No animations for enemy type!");
        return;

    case EnemyType::SkeletonBasic:
        anim_.AddFrame(sf::Sprite(GameAssets::Get().enemySpriteSheet, sf::IntRect(0, 0, 16, 16)), sf::seconds(0.2f));
        anim_.AddFrame(sf::Sprite(GameAssets::Get().enemySpriteSheet, sf::IntRect(16, 0, 16, 16)), sf::seconds(0.2f));
        break;

    case EnemyType::GreenBlobBasic:
        anim_.AddFrame(sf::Sprite(GameAssets::Get().enemySpriteSheet, sf::IntRect(0, 16, 16, 16)), sf::seconds(0.2f));
        anim_.AddFrame(sf::Sprite(GameAssets::Get().enemySpriteSheet, sf::IntRect(16, 16, 16, 16)), sf::seconds(0.2f));
        break;

    case EnemyType::BlueBlobBasic:
        anim_.AddFrame(sf::Sprite(GameAssets::Get().enemySpriteSheet, sf::IntRect(0, 32, 16, 16)), sf::seconds(0.2f));
        anim_.AddFrame(sf::Sprite(GameAssets::Get().enemySpriteSheet, sf::IntRect(16, 32, 16, 16)), sf::seconds(0.2f));
        break;

    case EnemyType::RedBlobBasic:
        anim_.AddFrame(sf::Sprite(GameAssets::Get().enemySpriteSheet, sf::IntRect(0, 48, 16, 16)), sf::seconds(0.2f));
        anim_.AddFrame(sf::Sprite(GameAssets::Get().enemySpriteSheet, sf::IntRect(16, 48, 16, 16)), sf::seconds(0.2f));
        break;

    case EnemyType::PinkBlobBasic:
        anim_.AddFrame(sf::Sprite(GameAssets::Get().enemySpriteSheet, sf::IntRect(0, 64, 16, 16)), sf::seconds(0.2f));
        anim_.AddFrame(sf::Sprite(GameAssets::Get().enemySpriteSheet, sf::IntRect(16, 64, 16, 16)), sf::seconds(0.2f));
        break;

    case EnemyType::GhostBasic:
        anim_.AddFrame(sf::Sprite(GameAssets::Get().enemySpriteSheet, sf::IntRect(0, 80, 16, 16)), sf::seconds(0.2f));
        anim_.AddFrame(sf::Sprite(GameAssets::Get().enemySpriteSheet, sf::IntRect(16, 80, 16, 16)), sf::seconds(0.2f));
        break;

    case EnemyType::MagicFlameBasic:
        anim_.AddFrame(sf::Sprite(GameAssets::Get().enemySpriteSheet, sf::IntRect(0, 96, 16, 16)), sf::seconds(0.2f));
        anim_.AddFrame(sf::Sprite(GameAssets::Get().enemySpriteSheet, sf::IntRect(16, 96, 16, 16)), sf::seconds(0.2f));
        break;
    }
}


void BasicEnemy::ResetStats(float difficultyMul)
{
    stats_ = std::make_unique<AliveStats>();

    switch (enemyType_) {
    default:
        assert(!"No stats for enemy type - using default!");
        break;

    case EnemyType::SkeletonBasic:
        stats_->SetMaxHealth(500 + static_cast<u32>(180 * difficultyMul));
        stats_->SetMoveSpeed(40.0f);
        stats_->SetMeleeAttack(90 + static_cast<u32>(40 * difficultyMul));
        stats_->SetMagicAttack(0);
        stats_->SetMeleeDefence(30 + static_cast<u32>(30 * difficultyMul));
        stats_->SetMagicDefence(0);
        break;

    case EnemyType::GreenBlobBasic:
        stats_->SetMaxHealth(150 + static_cast<u32>(80 * difficultyMul));
        stats_->SetMoveSpeed(40.0f);
        stats_->SetMeleeAttack(65 + static_cast<u32>(14 * difficultyMul));
        stats_->SetMagicAttack(0);
        stats_->SetMeleeDefence(static_cast<u32>(12 * difficultyMul));
        stats_->SetMagicDefence(static_cast<u32>(12 * difficultyMul));
        break;

    case EnemyType::BlueBlobBasic:
        stats_->SetMaxHealth(185 + static_cast<u32>(100 * difficultyMul));
        stats_->SetMoveSpeed(43.5f);
        stats_->SetMeleeAttack(70 + static_cast<u32>(22 * difficultyMul));
        stats_->SetMagicAttack(0);
        stats_->SetMeleeDefence(static_cast<u32>(20 * difficultyMul));
        stats_->SetMagicDefence(static_cast<u32>(20 * difficultyMul));
        break;

    case EnemyType::RedBlobBasic:
        stats_->SetMaxHealth(200 + static_cast<u32>(125 * difficultyMul));
        stats_->SetMoveSpeed(47.0f);
        stats_->SetMeleeAttack(75 + static_cast<u32>(30 * difficultyMul));
        stats_->SetMagicAttack(0);
        stats_->SetMeleeDefence(static_cast<u32>(28 * difficultyMul));
        stats_->SetMagicDefence(static_cast<u32>(28 * difficultyMul));
        break;

    case EnemyType::PinkBlobBasic:
        stats_->SetMaxHealth(230 + static_cast<u32>(170 * difficultyMul));
        stats_->SetMoveSpeed(50.5f);
        stats_->SetMeleeAttack(80 + static_cast<u32>(38 * difficultyMul));
        stats_->SetMagicAttack(0);
        stats_->SetMeleeDefence(static_cast<u32>(36 * difficultyMul));
        stats_->SetMagicDefence(static_cast<u32>(36 * difficultyMul));
        break;

    case EnemyType::GhostBasic:
        stats_->SetMaxHealth(600 + static_cast<u32>(220 * difficultyMul));
        stats_->SetMoveSpeed(30.0f);
        stats_->SetMeleeAttack(100 + static_cast<u32>(50 * difficultyMul));
        stats_->SetMagicAttack(0);
        stats_->SetMeleeDefence(80 + static_cast<u32>(60 * difficultyMul));
        stats_->SetMagicDefence(0);
        break;

    case EnemyType::MagicFlameBasic:
        stats_->SetMaxHealth(50 + static_cast<u32>(45 * difficultyMul));
        stats_->SetMoveSpeed(70.0f);
        stats_->SetMeleeAttack(0);
        stats_->SetMagicAttack(115 + static_cast<u32>(55 * difficultyMul));
        stats_->SetMeleeDefence(85 + static_cast<u32>(62 * difficultyMul));
        stats_->SetMagicDefence(20 + static_cast<u32>(8 * difficultyMul));
        break;
    }

    stats_->SetHealth(stats_->GetMaxHealth());
    SetStats(stats_.get());
}


float BasicEnemy::GetAggroDistance() const
{
    switch (enemyType_) {
    default:
        return 0.0f;

    case EnemyType::SkeletonBasic:
        return 90.0f;

    case EnemyType::MagicFlameBasic:
        return 58.0f;

    case EnemyType::GhostBasic:
        return 200.0f;
    }
}


void BasicEnemy::ResetStats()
{
    BasicEnemy::ResetStats(Game::Get().GetDirector().GetCurrentDifficultyMultiplier());
}


void BasicEnemy::NewWander()
{
    if (Helper::GenerateRandomBool(0.5)) {
        moveDir_ = sf::Vector2f();
    }
    else {
        moveDir_ = sf::Vector2f(Helper::GenerateRandomBool(0.5) ? 1.0f : -1.0f,
            Helper::GenerateRandomBool(0.5) ? 1.0f : -1.0f);
    }

    nextWanderMoveTime_ = sf::seconds(Helper::GenerateRandomReal(1.0f, 4.0f));
}


void BasicEnemy::HandleDropItems()
{
    if (droppedItems_) {
        return;
    }

    std::vector<std::unique_ptr<Item>> itemsToDrop;

    // drop tables
    switch (enemyType_) {
    case EnemyType::GreenBlobBasic:
        // potions
        if (Helper::GenerateRandomBool(1 / 16.0f)) {
            itemsToDrop.emplace_back(std::make_unique<PotionItem>(ItemType::HealthPotion, 1));
        }
        if (Helper::GenerateRandomBool(1 / 16.0f)) {
            itemsToDrop.emplace_back(std::make_unique<PotionItem>(ItemType::MagicPotion, 1));
        }

        // rare weapon
        if (Helper::GenerateRandomBool(1 / 150.0f)) {
            auto weapon = std::make_unique<MeleeWeapon>(MeleeWeaponType::AntiBlobSpear);
            weapon->SetDifficultyMultiplier(Game::Get().GetDirector().GetCurrentDifficultyMultiplier());
            itemsToDrop.emplace_back(std::move(weapon));
        }
        break;

    case EnemyType::BlueBlobBasic:
        // magic pot
        if (Helper::GenerateRandomBool(1 / 12.0f)) {
            itemsToDrop.emplace_back(std::make_unique<PotionItem>(ItemType::MagicPotion, 2));
        }

        // rare weapon
        if (Helper::GenerateRandomBool(1 / 100.0f)) {
            auto weapon = std::make_unique<MeleeWeapon>(MeleeWeaponType::AntiBlobSpear);
            weapon->SetDifficultyMultiplier(Game::Get().GetDirector().GetCurrentDifficultyMultiplier());
            itemsToDrop.emplace_back(std::move(weapon));
        }
        break;

    case EnemyType::RedBlobBasic:
        // health pot
        if (Helper::GenerateRandomBool(1 / 12.0f)) {
            itemsToDrop.emplace_back(std::make_unique<PotionItem>(ItemType::HealthPotion, 2));
        }

        // rare weapon
        if (Helper::GenerateRandomBool(1 / 75.0f)) {
            auto weapon = std::make_unique<MeleeWeapon>(MeleeWeaponType::AntiBlobSpear);
            weapon->SetDifficultyMultiplier(Game::Get().GetDirector().GetCurrentDifficultyMultiplier());
            itemsToDrop.emplace_back(std::move(weapon));
        }
        break;

    case EnemyType::PinkBlobBasic:
        // potions
        if (Helper::GenerateRandomBool(1 / 10.0f)) {
            itemsToDrop.emplace_back(std::make_unique<PotionItem>(ItemType::HealthPotion, 1));
        }
        if (Helper::GenerateRandomBool(1 / 10.0f)) {
            itemsToDrop.emplace_back(std::make_unique<PotionItem>(ItemType::MagicPotion, 1));
        }

        // rare weapon
        if (Helper::GenerateRandomBool(1 / 50.0f)) {
            auto weapon = std::make_unique<MeleeWeapon>(MeleeWeaponType::AntiBlobSpear);
            weapon->SetDifficultyMultiplier(Game::Get().GetDirector().GetCurrentDifficultyMultiplier());
            itemsToDrop.emplace_back(std::move(weapon));
        }
        break;

    case EnemyType::SkeletonBasic:
        // potions
        if (Helper::GenerateRandomBool(1 / 7.0f)) {
            itemsToDrop.emplace_back(std::make_unique<PotionItem>(ItemType::HealthPotion, 
                Helper::GenerateRandomBool(1 / 7.0f) ? 2 : 1));
        }
        if (Helper::GenerateRandomBool(1 / 7.0f)) {
            itemsToDrop.emplace_back(std::make_unique<PotionItem>(ItemType::MagicPotion,
                Helper::GenerateRandomBool(1 / 7.0f) ? 2 : 1));
        }

        // roll for melee weapon
        if (Helper::GenerateRandomBool(1 / 10.0f)) {
            std::unique_ptr<MeleeWeapon> weapon;

            auto randNum = Helper::GenerateRandomInt(1, 100);

            if (randNum <= 5) {
                // rare drop!
                weapon = std::make_unique<MeleeWeapon>(MeleeWeaponType::Zeraleth);
            }
            else if (randNum <= 50) {
                weapon = std::make_unique<MeleeWeapon>(MeleeWeaponType::AdventurerSword);
            }
            else if (randNum <= 60) {
                weapon = std::make_unique<MeleeWeapon>(MeleeWeaponType::RoguesDagger);
            }
            else if (randNum <= 70) {
                weapon = std::make_unique<MeleeWeapon>(MeleeWeaponType::GraniteBlade);
            }
            else if (randNum <= 80) {
                weapon = std::make_unique<MeleeWeapon>(MeleeWeaponType::ShardBlade);
            }
            else if (randNum <= 90) {
                weapon = std::make_unique<MeleeWeapon>(MeleeWeaponType::RegenBlade);
            }
            else {
                weapon = std::make_unique<MeleeWeapon>(MeleeWeaponType::ThornedSabre);
            }

            weapon->SetDifficultyMultiplier(Game::Get().GetDirector().GetCurrentDifficultyMultiplier());
            itemsToDrop.emplace_back(std::move(weapon));
        }

        // roll for melee armour
        if (Helper::GenerateRandomBool(1 / 18.0f)) {
            auto armour = std::make_unique<Armour>(ArmourType::WarriorHelmet);
            armour->SetDifficultyMultiplier(Game::Get().GetDirector().GetCurrentDifficultyMultiplier());
            itemsToDrop.emplace_back(std::move(armour));
        }
        break;

    case EnemyType::GhostBasic:
        // potions
        if (Helper::GenerateRandomBool(1 / 5.0f)) {
            itemsToDrop.emplace_back(std::make_unique<PotionItem>(ItemType::HealthPotion,
                Helper::GenerateRandomBool(1 / 3.0f) ? 2 : 1));
        }
        if (Helper::GenerateRandomBool(1 / 5.0f)) {
            itemsToDrop.emplace_back(std::make_unique<PotionItem>(ItemType::MagicPotion,
                Helper::GenerateRandomBool(1 / 3.0f) ? 2 : 1));
        }

        // roll for melee weapon
        if (Helper::GenerateRandomBool(1 / 8.0f)) {
            std::unique_ptr<MeleeWeapon> weapon;

            auto randNum = Helper::GenerateRandomInt(1, 100);

            if (randNum <= 10) {
                // rare drop!
                weapon = std::make_unique<MeleeWeapon>(MeleeWeaponType::Zeraleth);
            }
            else if (randNum <= 50) {
                weapon = std::make_unique<MeleeWeapon>(MeleeWeaponType::AdventurerSword);
            }
            else if (randNum <= 60) {
                weapon = std::make_unique<MeleeWeapon>(MeleeWeaponType::RoguesDagger);
            }
            else if (randNum <= 70) {
                weapon = std::make_unique<MeleeWeapon>(MeleeWeaponType::GraniteBlade);
            }
            else if (randNum <= 80) {
                weapon = std::make_unique<MeleeWeapon>(MeleeWeaponType::ShardBlade);
            }
            else if (randNum <= 90) {
                weapon = std::make_unique<MeleeWeapon>(MeleeWeaponType::RegenBlade);
            }
            else {
                weapon = std::make_unique<MeleeWeapon>(MeleeWeaponType::ThornedSabre);
            }

            weapon->SetDifficultyMultiplier(Game::Get().GetDirector().GetCurrentDifficultyMultiplier());
            itemsToDrop.emplace_back(std::move(weapon));
        }

        // roll for melee armour
        if (Helper::GenerateRandomBool(1 / 12.0f)) {
            auto armour = std::make_unique<Armour>(ArmourType::WarriorHelmet);
            armour->SetDifficultyMultiplier(Game::Get().GetDirector().GetCurrentDifficultyMultiplier());
            itemsToDrop.emplace_back(std::move(armour));
        }
        break;

    case EnemyType::MagicFlameBasic:
        // potions
        if (Helper::GenerateRandomBool(1 / 8.0f)) {
            itemsToDrop.emplace_back(std::make_unique<PotionItem>(ItemType::HealthPotion, 2));
        }
        if (Helper::GenerateRandomBool(1 / 8.0f)) {
            itemsToDrop.emplace_back(std::make_unique<PotionItem>(ItemType::MagicPotion, 2));
        }

        // roll for magic weapon
        if (Helper::GenerateRandomBool(1 / 12.0f)) {
            std::unique_ptr<MagicWeapon> weapon;

            auto randNum = Helper::GenerateRandomInt(1, 100);

            if (randNum <= 60) {
                weapon = std::make_unique<MagicWeapon>(MagicWeaponType::FlameStaff);
            }
            else {
                weapon = std::make_unique<MagicWeapon>(MagicWeaponType::DrainStaff);
            }

            weapon->SetDifficultyMultiplier(Game::Get().GetDirector().GetCurrentDifficultyMultiplier());
            itemsToDrop.emplace_back(std::move(weapon));
        }

        // roll for magic armour
        if (Helper::GenerateRandomBool(1 / 12.0f)) {
            auto armour = std::make_unique<Armour>(ArmourType::AntiMagicVisor);
            armour->SetDifficultyMultiplier(Game::Get().GetDirector().GetCurrentDifficultyMultiplier());
            itemsToDrop.emplace_back(std::move(armour));
        }
        break;

        // TODO give harder monsters chance to drop balance gear & staff of protection
    }

    // drop items below us
    for (auto& item : itemsToDrop) {
        auto itemEnt = GetAssignedArea()->GetEntity<ItemEntity>(GetAssignedArea()->EmplaceEntity<ItemEntity>());
        itemEnt->SetItem(std::move(item));
        itemEnt->SetCenterPosition(GetCenterPosition() +
            sf::Vector2f(Helper::GenerateRandomReal(-8.0f, 8.0f), 0.5f * (GetSize().y - itemEnt->GetSize().y)));
    }

    droppedItems_ = true;
}


void BasicEnemy::Tick()
{
    auto stats = GetStats();
    auto area = GetAssignedArea();

    if (stats && area) {
        if (stats->IsAlive()) {
            MoveWithCollision(moveDir_ * stats->GetMoveSpeed() * Game::FrameTimeStep.asSeconds());

            // check if player is in aggro range and aggro on player if they are
            auto closePlayers = area->GetWorldEntitiesInRange<PlayerEntity>(GetCenterPosition(), GetAggroDistance());
            float playerDistSq = GetAggroDistance() * GetAggroDistance() + 1.0f;
            PlayerEntity* playerAggro = nullptr;

            for (auto pinfo : closePlayers) {
                auto player = area->GetEntity<PlayerEntity>(pinfo.first);
                assert(player);

                if (player->GetStats() && player->GetStats()->IsAlive() && pinfo.second < playerDistSq) {
                    playerDistSq = pinfo.second;
                    playerAggro = player;
                }
            }

            if (playerAggro) {
                // aggro on this player
                auto aggroMove = playerAggro->GetCenterPosition() - GetCenterPosition();
                aggroMove /= sqrtf(aggroMove.x * aggroMove.x + aggroMove.y * aggroMove.y);
                moveDir_ = aggroMove;
            }
            else {
                // wander
                if (nextWanderMoveTime_ <= sf::Time::Zero) {
                    NewWander();
                }
                else {
                    nextWanderMoveTime_ -= Game::FrameTimeStep;
                }
            }

            if (moveDir_.x != 0.0f || moveDir_.y != 0.0f || enemyType_ == EnemyType::MagicFlameBasic) {
                anim_.Tick();
            }

            // damage players touching us
            auto touchingPlayers = area->GetAllWorldEntsInRectangle<PlayerEntity>(GetRectangle());

            for (auto pid : touchingPlayers) {
                auto player = area->GetEntity<PlayerEntity>(pid);
                assert(player);

                if (player->GetStats() && player->GetStats()->IsAlive() && !player->HasInvincibility()) {
                    switch (enemyType_) {
                    default:
                        player->Attack(Helper::GenerateRandomInt<u32>(0, stats->GetMeleeAttack()), DamageType::Melee);
                        break;

                    case EnemyType::MagicFlameBasic:
                        player->Attack(Helper::GenerateRandomInt<u32>(0, stats->GetMagicAttack()), DamageType::Magic);
                        break;
                    }
                    player->MoveWithCollision(moveDir_ * 5.0f); // push player
                }
            }
        }
        else if (!droppedItems_) {
            // dead - drop if we havent already
            HandleDropItems();
        }
    }
}


void BasicEnemy::Render(sf::RenderTarget& target)
{
    auto stats = GetStats();

    if (stats) {
        // render sprite
        sf::Sprite enemySprite;

        if (stats->IsAlive()) {
            // alive sprites
            enemySprite = anim_.GetCurrentFrame();
        }
        else {
            // dead sprites
            enemySprite.setTexture(GameAssets::Get().enemySpriteSheet);

            switch (enemyType_) {
            case EnemyType::SkeletonBasic:
                enemySprite.setTextureRect(sf::IntRect(32, 0, 16, 16));
                break;

            case EnemyType::GreenBlobBasic:
                enemySprite.setTextureRect(sf::IntRect(32, 16, 16, 16));
                break;

            case EnemyType::BlueBlobBasic:
                enemySprite.setTextureRect(sf::IntRect(32, 32, 16, 16));
                break;

            case EnemyType::RedBlobBasic:
                enemySprite.setTextureRect(sf::IntRect(32, 48, 16, 16));
                break;

            case EnemyType::PinkBlobBasic:
                enemySprite.setTextureRect(sf::IntRect(32, 64, 16, 16));
                break;

            case EnemyType::GhostBasic:
                enemySprite.setTextureRect(sf::IntRect(32, 80, 16, 16));
                break;

            case EnemyType::MagicFlameBasic:
                enemySprite.setTextureRect(sf::IntRect(32, 96, 16, 16));
                break;
            }
        }

        enemySprite.setPosition(GetPosition());
        target.draw(enemySprite);

        // render stats
        Enemy::Render(target);
    }
}


std::string BasicEnemy::GetUnitName() const
{
    switch (enemyType_) {
    default:
        return std::string();

    case EnemyType::SkeletonBasic:
        return "Skeleton";

    case EnemyType::GreenBlobBasic:
        return "Green Blob";

    case EnemyType::BlueBlobBasic:
        return "Blue Blob";

    case EnemyType::RedBlobBasic:
        return "Red Blob";

    case EnemyType::PinkBlobBasic:
        return "Pink Blob";

    case EnemyType::GhostBasic:
        return "Ghost";

    case EnemyType::MagicFlameBasic:
        return "Magic Flame";
    }
}