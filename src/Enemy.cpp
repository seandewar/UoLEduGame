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
            healthBarBg->setPosition(GetPosition() - sf::Vector2f(3.0f, 4.0f));
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
            healthLabelText->setScale(0.15f, 0.15f);
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
enemyType_(enemyType)
{
    if (enemyType_ != EnemyType::SkeletonBasic &&
        enemyType_ != EnemyType::GreenBlobBasic &&
        enemyType_ != EnemyType::BlueBlobBasic &&
        enemyType_ != EnemyType::RedBlobBasic &&
        enemyType_ != EnemyType::PinkBlobBasic) {
        throw std::runtime_error("Bad BasicEnemy enemy type!");
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
        anim_.AddFrame(sf::Sprite(GameAssets::Get().enemySpriteSheet, sf::IntRect(0, 0, 16, 16)), sf::milliseconds(200));
        anim_.AddFrame(sf::Sprite(GameAssets::Get().enemySpriteSheet, sf::IntRect(16, 0, 16, 16)), sf::milliseconds(200));
        break;

    case EnemyType::GreenBlobBasic:
        anim_.AddFrame(sf::Sprite(GameAssets::Get().enemySpriteSheet, sf::IntRect(0, 16, 16, 16)), sf::milliseconds(200));
        anim_.AddFrame(sf::Sprite(GameAssets::Get().enemySpriteSheet, sf::IntRect(16, 16, 16, 16)), sf::milliseconds(200));
        break;

    case EnemyType::BlueBlobBasic:
        anim_.AddFrame(sf::Sprite(GameAssets::Get().enemySpriteSheet, sf::IntRect(0, 32, 16, 16)), sf::milliseconds(200));
        anim_.AddFrame(sf::Sprite(GameAssets::Get().enemySpriteSheet, sf::IntRect(16, 32, 16, 16)), sf::milliseconds(200));
        break;

    case EnemyType::RedBlobBasic:
        anim_.AddFrame(sf::Sprite(GameAssets::Get().enemySpriteSheet, sf::IntRect(0, 48, 16, 16)), sf::milliseconds(200));
        anim_.AddFrame(sf::Sprite(GameAssets::Get().enemySpriteSheet, sf::IntRect(16, 48, 16, 16)), sf::milliseconds(200));
        break;

    case EnemyType::PinkBlobBasic:
        anim_.AddFrame(sf::Sprite(GameAssets::Get().enemySpriteSheet, sf::IntRect(0, 64, 16, 16)), sf::milliseconds(200));
        anim_.AddFrame(sf::Sprite(GameAssets::Get().enemySpriteSheet, sf::IntRect(16, 64, 16, 16)), sf::milliseconds(200));
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
        stats_->SetMaxHealth(500 + static_cast<u32>(150 * difficultyMul));
        stats_->SetMoveSpeed(40.0f);
        stats_->SetMeleeAttack(85 + static_cast<u32>(20 * difficultyMul));
        stats_->SetMagicAttack(0);
        stats_->SetMeleeDefence(20 + static_cast<u32>(10 * difficultyMul));
        stats_->SetMagicDefence(0);
        break;

    case EnemyType::GreenBlobBasic:
        stats_->SetMaxHealth(150 + static_cast<u32>(80 * difficultyMul));
        stats_->SetMoveSpeed(40.0f);
        stats_->SetMeleeAttack(50 + static_cast<u32>(10 * difficultyMul));
        stats_->SetMagicAttack(0);
        stats_->SetMeleeDefence(static_cast<u32>(8 * difficultyMul));
        stats_->SetMagicDefence(0);
        break;

    case EnemyType::BlueBlobBasic:
        stats_->SetMaxHealth(185 + static_cast<u32>(95 * difficultyMul));
        stats_->SetMoveSpeed(43.5f);
        stats_->SetMeleeAttack(50 + static_cast<u32>(10 * difficultyMul));
        stats_->SetMagicAttack(0);
        stats_->SetMeleeDefence(static_cast<u32>(8 * difficultyMul));
        stats_->SetMagicDefence(0);
        break;

    case EnemyType::RedBlobBasic:
        stats_->SetMaxHealth(200 + static_cast<u32>(110 * difficultyMul));
        stats_->SetMoveSpeed(47.0f);
        stats_->SetMeleeAttack(50 + static_cast<u32>(10 * difficultyMul));
        stats_->SetMagicAttack(0);
        stats_->SetMeleeDefence(static_cast<u32>(8 * difficultyMul));
        stats_->SetMagicDefence(0);
        break;

    case EnemyType::PinkBlobBasic:
        stats_->SetMaxHealth(230 + static_cast<u32>(135 * difficultyMul));
        stats_->SetMoveSpeed(50.5f);
        stats_->SetMeleeAttack(50 + static_cast<u32>(10 * difficultyMul));
        stats_->SetMagicAttack(0);
        stats_->SetMeleeDefence(static_cast<u32>(8 * difficultyMul));
        stats_->SetMagicDefence(0);
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
        return 100.0f;
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


void BasicEnemy::Tick()
{
    auto stats = GetStats();

    if (stats && stats->IsAlive()) {
        MoveWithCollision(moveDir_ * stats->GetMoveSpeed() * Game::FrameTimeStep.asSeconds());

        if (nextWanderMoveTime_ <= sf::Time::Zero) {
            NewWander();
        }
        else {
            nextWanderMoveTime_ -= Game::FrameTimeStep;
        }

        if (moveDir_.x != 0.0f || moveDir_.y != 0.0f) {
            anim_.Tick();
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
    }
}