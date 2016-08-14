#include "Enemy.h"

#include <cassert>

#include <SFML/Graphics/RectangleShape.hpp>

#include "Game.h"
#include "Helper.h"
#include "Projectile.h"


Enemy::Enemy() :
AliveEntity()
{
    SetSize(sf::Vector2f(16.0f, 16.0f));
}


Enemy::~Enemy()
{
}


u32 Enemy::Damage(u32 damageAmount, DamageType type)
{
    Game::Get().NotifyPlayerDamageGiven(damageAmount);
    return AliveEntity::Damage(damageAmount, type);
}


void Enemy::Render(sf::RenderTarget& target)
{
    auto area = GetAssignedArea();

    if (!area || Game::Get().IsInMapMode()) {
        return;
    }

    // render stats
    auto stats = GetStats();

    if (stats && stats->IsAlive()) {
        auto health = stats->GetHealth();
        auto maxHealth = stats->GetMaxHealth();

        if (maxHealth > 0) {
            // render health bar bg
            auto healthBarBg = std::make_unique<sf::RectangleShape>(sf::Vector2f(26.0f, 3.0f));
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
            auto healthLabelText = std::make_unique<sf::Text>(std::string("H: ") + std::to_string(stats->GetHealth()),
                GameAssets::Get().gameFont, 12);
            healthLabelText->setScale(0.2f, 0.25f);
            healthLabelText->setPosition(healthBarBg->getPosition() + 0.5f * healthBarBg->getSize() - 0.5f *
                sf::Vector2f(healthLabelText->getGlobalBounds().width, healthLabelText->getGlobalBounds().height));
            healthLabelText->setColor(sf::Color(255, 255, 255));

            auto healthLabelShadow = Helper::GetTextDropShadow(*healthLabelText, sf::Vector2f(0.2f, 0.2f));

            // render name text label
            auto nameText = std::make_unique<sf::Text>(GetUnitName(), GameAssets::Get().gameFont, 12);
            nameText->setScale(0.2f, 0.25f);
            nameText->setPosition(healthBarBg->getPosition() + 0.5f * healthBarBg->getSize() - 0.5f *
                sf::Vector2f(nameText->getGlobalBounds().width, 10.0f));
            nameText->setColor(sf::Color(255, 255, 255));

            auto nameTextShadow = Helper::GetTextDropShadow(*nameText, sf::Vector2f(0.2f, 0.2f));

            area->AddFrameUIRenderable(healthBarBg);
            area->AddFrameUIRenderable(healthBarFg);
            area->AddFrameUIRenderable(healthLabelShadow);
            area->AddFrameUIRenderable(healthLabelText);
            area->AddFrameUIRenderable(nameTextShadow);
            area->AddFrameUIRenderable(nameText);
        }
    }
}


DungeonGuardian::DungeonGuardian() :
Enemy(),
form_(DungeonGuardianForm::MagicForm),
rot_(Helper::GenerateRandomReal(0.0f, 360.0f)),
handleDeathAnim_(false)
{
    SetSize(sf::Vector2f(32.0f, 32.0f));

    SetupAnimations();
    ResetStats(); 
    ChangeForm(DungeonGuardianForm::MagicForm);

    GameAssets::Get().bossSpawnSound.play();
}


DungeonGuardian::~DungeonGuardian()
{
}


void DungeonGuardian::SetupAnimations()
{
    animMagicForm_.AddFrame(sf::Sprite(GameAssets::Get().enemySpriteSheet, sf::IntRect(48, 0, 32, 32)), sf::seconds(0.2f));
    animMagicForm_.AddFrame(sf::Sprite(GameAssets::Get().enemySpriteSheet, sf::IntRect(80, 0, 32, 32)), sf::seconds(0.2f));
    animMagicForm_.AddFrame(sf::Sprite(GameAssets::Get().enemySpriteSheet, sf::IntRect(112, 0, 32, 32)), sf::seconds(0.2f));
    animMagicForm_.AddFrame(sf::Sprite(GameAssets::Get().enemySpriteSheet, sf::IntRect(80, 0, 32, 32)), sf::seconds(0.2f));

    animSmokeForm_.AddFrame(sf::Sprite(GameAssets::Get().enemySpriteSheet, sf::IntRect(48, 32, 32, 32)), sf::seconds(0.2f));
    animSmokeForm_.AddFrame(sf::Sprite(GameAssets::Get().enemySpriteSheet, sf::IntRect(80, 32, 32, 32)), sf::seconds(0.2f));
    animSmokeForm_.AddFrame(sf::Sprite(GameAssets::Get().enemySpriteSheet, sf::IntRect(112, 32, 32, 32)), sf::seconds(0.2f));
    animSmokeForm_.AddFrame(sf::Sprite(GameAssets::Get().enemySpriteSheet, sf::IntRect(80, 32, 32, 32)), sf::seconds(0.2f));

    animMeleeForm_.AddFrame(sf::Sprite(GameAssets::Get().enemySpriteSheet, sf::IntRect(48, 64, 32, 32)), sf::seconds(0.2f));
    animMeleeForm_.AddFrame(sf::Sprite(GameAssets::Get().enemySpriteSheet, sf::IntRect(80, 64, 32, 32)), sf::seconds(0.2f));
    animMeleeForm_.AddFrame(sf::Sprite(GameAssets::Get().enemySpriteSheet, sf::IntRect(112, 64, 32, 32)), sf::seconds(0.2f));
}


void DungeonGuardian::ResetStats(float difficultyMul)
{
    stats_ = std::make_unique<AliveStats>();

    stats_->SetMaxHealth(1500 + static_cast<u32>(1000 * difficultyMul));
    stats_->SetMeleeAttack(90 + static_cast<u32>(40 * difficultyMul));
    stats_->SetMagicAttack(90 + static_cast<u32>(50 * difficultyMul));
    stats_->SetMeleeDefence(40 + static_cast<u32>(40 * difficultyMul));
    stats_->SetMagicDefence(100 + static_cast<u32>(145 * difficultyMul));

    stats_->SetHealth(stats_->GetMaxHealth());
    SetStats(stats_.get());
}


void DungeonGuardian::ResetStats()
{
    ResetStats(Game::Get().GetDirector().GetCurrentDifficultyMultiplier());
}


sf::Time DungeonGuardian::GetDefaultTimeForAction()
{
    switch (form_) {
    default:
        return sf::seconds(1.0f);

    case DungeonGuardianForm::MagicForm:
        return sf::seconds(2.5f);

    case DungeonGuardianForm::SmokeForm:
        return sf::seconds(5.0f);

    case DungeonGuardianForm::MeleeForm:
        return sf::seconds(7.5f);
    }
}


void DungeonGuardian::ChangeForm(DungeonGuardianForm newForm)
{
    auto stats = GetStats();

    if (!stats) {
        return;
    }

    switch (newForm) {
    default:
        formActionsLeft_ = 0;
        break;

    case DungeonGuardianForm::MagicForm:
        formActionsLeft_ = 5;
        break;

    case DungeonGuardianForm::SmokeForm:
        stats->SetMoveSpeed(20.0f);
        formActionsLeft_ = 5;
        break;

    case DungeonGuardianForm::MeleeForm:
        stats->SetMoveSpeed(70.0f);
        formActionsLeft_ = 1;
        break;
    }

    form_ = newForm;
    formSoundTimeLeft_ = sf::seconds(1.0f);
    NewFormAction();
    ResetAnimations();
}


void DungeonGuardian::ResetAnimations()
{
    animMagicForm_.Restart();
    animSmokeForm_.Restart();
    animMeleeForm_.Restart();
}


void DungeonGuardian::TickAnimations()
{
    animMagicForm_.Tick();
    animSmokeForm_.Tick();
    animMeleeForm_.Tick();
}


void DungeonGuardian::Tick()
{
    auto area = GetAssignedArea();
    auto stats = GetStats();

    if (!area || !stats) {
        return;
    }

    // if dead, handle death anim flags
    if (!stats->IsAlive()) {
        if (!handleDeathAnim_) {
            // handle death anim now by flashing for 4 seconds,
            // then explode into orbs for effect!
            actionTimeLeft_ = sf::seconds(3.0f);
            handleDeathAnim_ = true;

            GameAssets::Get().bossDyingSound.play();
        }
    }

    // get player to aggro on - aggro dist is largest dimension in area so all players
    // are considered
    auto aggroDist = std::max(BaseTile::TileSize.x * area->GetWidth(), BaseTile::TileSize.y * area->GetHeight());

    auto closePlayers = area->GetWorldEntitiesInRange<PlayerEntity>(GetCenterPosition(), aggroDist);
    float playerDistSq = aggroDist * aggroDist + 1.0f;
    PlayerEntity* playerAggro = nullptr;

    if (stats->IsAlive()) {
        for (auto pinfo : closePlayers) {
            auto player = area->GetEntity<PlayerEntity>(pinfo.first);
            assert(player);

            if (player->GetStats() && player->GetStats()->IsAlive() && pinfo.second < playerDistSq) {
                playerDistSq = pinfo.second;
                playerAggro = player;
            }
        }

        if (playerAggro) {
            // handle actions for forms
            if (!firedThisAction_ && actionTimeLeft_.asSeconds() < GetDefaultTimeForAction().asSeconds() * 0.5f) {
                switch (form_) {
                case DungeonGuardianForm::MagicForm: {
                    // fire projectile in general direction of aggro'd player
                    auto dirToPlayer = Helper::GetUnitVector(playerAggro->GetCenterPosition() - GetCenterPosition());
                    auto projectileDir = dirToPlayer + sf::Vector2f(Helper::GenerateRandomReal(-0.1f, 0.1f),
                        Helper::GenerateRandomReal(-0.1f, 0.1f));

                    auto projectile = area->GetEntity<ProjectileEntity>(
                        area->EmplaceEntity<ProjectileEntity>(ProjectileType::EnemyMagicFlame, projectileDir));
                    assert(projectile);

                    projectile->SetCenterPosition(GetCenterPosition() + projectileDir * 8.0f);
                    projectile->SetDamage(Helper::GenerateRandomInt<u32>(0, stats->GetMagicAttack()));

                    GameAssets::Get().magicFireSound.play();
                    break;
                }

                case DungeonGuardianForm::SmokeForm:
                    // fire projectiles in lots of directions around boss
                    int numProjectiles = 12;
                    auto projAngleRadsOffset = Helper::GenerateRandomReal(0.0f, 2.0f * PI);

                    for (int i = 0; i < numProjectiles; ++i) {
                        auto projAngleRads = 2.0f * PI * (i / static_cast<float>(numProjectiles)) + projAngleRadsOffset;
                        sf::Vector2f projectileDir(cosf(projAngleRads), sinf(projAngleRads));

                        auto projectile = area->GetEntity<ProjectileEntity>(
                            area->EmplaceEntity<ProjectileEntity>(ProjectileType::EnemySmoke, projectileDir));
                        assert(projectile);

                        projectile->SetCenterPosition(GetCenterPosition() + projectileDir * 8.0f);
                        projectile->SetDamage(Helper::GenerateRandomInt<u32>(stats->GetMagicAttack() / 4,
                            stats->GetMagicAttack()));

                        GameAssets::Get().smokeSound.play();
                    }
                    break;
                }

                firedThisAction_ = true;
            }

            // move depending on form
            switch (form_) {
            case DungeonGuardianForm::MeleeForm:
            case DungeonGuardianForm::SmokeForm:
                moveDir_ = Helper::GetUnitVector(
                    moveDir_ + 0.1f * Helper::GetUnitVector(playerAggro->GetCenterPosition() - GetCenterPosition()));
                break;

            case DungeonGuardianForm::MagicForm:
                moveDir_ = sf::Vector2f();
                break;
            }
        }

        // apply move
        Move(moveDir_ * stats->GetMoveSpeed() * Game::FrameTimeStep.asSeconds());

        // damage players touching us depending on form
        auto touchingPlayers = area->GetAllWorldEntsInRectangle<PlayerEntity>(GetRectangle());

        for (auto pid : touchingPlayers) {
            auto player = area->GetEntity<PlayerEntity>(pid);
            assert(player);

            if (player->GetStats() && player->GetStats()->IsAlive() && !player->HasInvincibility()) {
                switch (form_) {
                case DungeonGuardianForm::MeleeForm:
                    player->Attack(Helper::GenerateRandomInt<u32>(0, stats->GetMeleeAttack()), DamageType::Melee);
                    break;

                case DungeonGuardianForm::MagicForm:
                    player->Attack(Helper::GenerateRandomInt<u32>(0, stats->GetMeleeAttack() / 2), DamageType::Magic);
                    break;

                case DungeonGuardianForm::SmokeForm:
                    player->Attack(Helper::GenerateRandomInt<u32>(stats->GetMeleeAttack() / 4, stats->GetMeleeAttack() / 2),
                        DamageType::Other);
                    break;
                }
                player->MoveWithCollision(moveDir_ * 5.0f); // push player
            }
        }

        // tick animss
        TickAnimations();
        rot_ += 500.0f * Game::FrameTimeStep.asSeconds();
    }

    // handle form time tick & switch
    if (actionTimeLeft_ <= sf::Time::Zero) {
        if (stats->IsAlive()) {
            // handle next action / form switching if alive
            if (--formActionsLeft_ <= 0) {
                // next form
                switch (form_) {
                case DungeonGuardianForm::MagicForm:
                    ChangeForm(DungeonGuardianForm::SmokeForm);
                    break;

                case DungeonGuardianForm::SmokeForm:
                    ChangeForm(DungeonGuardianForm::MeleeForm);
                    break;

                default:
                case DungeonGuardianForm::MeleeForm:
                    ChangeForm(DungeonGuardianForm::MagicForm);
                    break;
                }
            }
            else {
                NewFormAction();
            }

            GameAssets::Get().bossActionSound.play();

            // tp in rad around player if player exists
            auto center = playerAggro ? playerAggro->GetCenterPosition() : GetCenterPosition();
            SetCenterPosition(center + Helper::GenerateRandomReal(75.0f, 110.0f) * Helper::GetUnitVector(
                sf::Vector2f(Helper::GenerateRandomReal(-1.0f, 1.0f), Helper::GenerateRandomReal(-1.0f, 1.0f))));
        }
        else {
            // handle death and explode into orbs for effect
            // and mark ent delete
            int numProjectiles = 20;

            for (int i = 0; i < numProjectiles; ++i) {
                auto projAngleRads = 2.0f * PI * (i / static_cast<float>(numProjectiles));
                sf::Vector2f projectileDir(cosf(projAngleRads), sinf(projAngleRads));

                auto projectile = area->GetEntity<ProjectileEntity>(
                    area->EmplaceEntity<ProjectileEntity>(ProjectileType::EffectOrb, projectileDir));
                assert(projectile);

                projectile->SetCenterPosition(GetCenterPosition() + projectileDir * 12.0f);
            }

            GameAssets::Get().bossDeadSound.play();
            Game::Get().GetDirector().BossDefeated();

            MarkForDeletion();
            return;
        }
    }
    else {
        actionTimeLeft_ -= Game::FrameTimeStep;
    }

    // handle form sound timer NOTE: (only used for melee form right now, may or may not be used)
    // for other forms
    if (formSoundTimeLeft_ <= sf::Time::Zero) {
        switch (form_) {
        case DungeonGuardianForm::MeleeForm:
            GameAssets::Get().bossSwordSound.play();
            break;
        }

        formSoundTimeLeft_ = sf::seconds(0.8f);
    }
    else {
        formSoundTimeLeft_ -= Game::FrameTimeStep;
    }
}


void DungeonGuardian::Render(sf::RenderTarget& target)
{
    auto area = GetAssignedArea();

    if (!area) {
        return;
    }

    // render boss
    std::unique_ptr<sf::Sprite> bossSprite;

    switch (form_) {
    case DungeonGuardianForm::MagicForm:
        bossSprite = std::make_unique<sf::Sprite>(animMagicForm_.GetCurrentFrame());
        break;

    case DungeonGuardianForm::SmokeForm:
        bossSprite = std::make_unique<sf::Sprite>(animSmokeForm_.GetCurrentFrame());
        break;

    case DungeonGuardianForm::MeleeForm:
        bossSprite = std::make_unique<sf::Sprite>(animMeleeForm_.GetCurrentFrame());
        bossSprite->setRotation(rot_);
        break;
    }

    bossSprite->setOrigin(0.5f * GetSize());
    bossSprite->setPosition(GetCenterPosition());

    // calc fade amount
    float opacityMul = 1.0f;

    if (GetStats() && !GetStats()->IsAlive()) {
        // dead - flash
        bossSprite->setColor(sf::Color(
            Helper::GenerateRandomInt(0, 255),
            Helper::GenerateRandomInt(0, 255),
            Helper::GenerateRandomInt(0, 255)));
    }
    else {
        // alive
        if (actionTimeLeft_ <= sf::seconds(0.75f)) {
            opacityMul = std::max(0.0f, actionTimeLeft_.asSeconds() / 0.75f);
        }
        else if (actionTimeLeft_ >= GetDefaultTimeForAction() - sf::seconds(0.75f)) {
            opacityMul = std::max(0.0f, (GetDefaultTimeForAction() - actionTimeLeft_).asSeconds() / 0.75f);
        }

        bossSprite->setColor(sf::Color(255, 255, 255, static_cast<sf::Uint8>(opacityMul * 255)));
    }

    // calc sprite size amount
    // oversize to make hitbox seem encapsulated
    float scale = std::min(form_ == DungeonGuardianForm::MeleeForm ? 1.25f : 2.0f, opacityMul * 2.15f);
    bossSprite->setScale(scale, scale);

    // render dead effect
    if (GetStats() && !GetStats()->IsAlive()) {
        area->AddFrameUIRenderable(bossSprite);
    }
    else {
        target.draw(*bossSprite);

        // render stats
        Enemy::Render(target);
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
        enemyType_ != EnemyType::MagicFlameBasic &&
        enemyType_ != EnemyType::AncientWizardBasic &&
        enemyType_ != EnemyType::DarkWizardBasic) {
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
    case EnemyType::AncientWizardBasic:
    case EnemyType::DarkWizardBasic:
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

    case EnemyType::AncientWizardBasic:
        anim_.AddFrame(sf::Sprite(GameAssets::Get().enemySpriteSheet, sf::IntRect(0, 112, 16, 16)), sf::seconds(0.4f));
        anim_.AddFrame(sf::Sprite(GameAssets::Get().enemySpriteSheet, sf::IntRect(16, 112, 16, 16)), sf::seconds(0.4f));
        break;

    case EnemyType::DarkWizardBasic:
        anim_.AddFrame(sf::Sprite(GameAssets::Get().enemySpriteSheet, sf::IntRect(0, 128, 16, 16)), sf::seconds(0.4f));
        anim_.AddFrame(sf::Sprite(GameAssets::Get().enemySpriteSheet, sf::IntRect(16, 128, 16, 16)), sf::seconds(0.4f));
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
        stats_->SetMoveSpeed(42.5f);
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
        stats_->SetMaxHealth(600 + static_cast<u32>(200 * difficultyMul));
        stats_->SetMoveSpeed(32.5f);
        stats_->SetMeleeAttack(100 + static_cast<u32>(50 * difficultyMul));
        stats_->SetMagicAttack(0);
        stats_->SetMeleeDefence(80 + static_cast<u32>(60 * difficultyMul));
        stats_->SetMagicDefence(0);
        break;

    case EnemyType::MagicFlameBasic:
        stats_->SetMaxHealth(50 + static_cast<u32>(45 * difficultyMul));
        stats_->SetMoveSpeed(67.5f);
        stats_->SetMeleeAttack(0);
        stats_->SetMagicAttack(115 + static_cast<u32>(55 * difficultyMul));
        stats_->SetMeleeDefence(85 + static_cast<u32>(62 * difficultyMul));
        stats_->SetMagicDefence(20 + static_cast<u32>(8 * difficultyMul));
        break;

    case EnemyType::AncientWizardBasic:
        stats_->SetMaxHealth(275 + static_cast<u32>(135 * difficultyMul));
        stats_->SetMoveSpeed(40.0f);
        stats_->SetMeleeAttack(65 + static_cast<u32>(35 * difficultyMul));
        stats_->SetMagicAttack(90 + static_cast<u32>(48 * difficultyMul));
        stats_->SetMeleeDefence(20 + static_cast<u32>(20 * difficultyMul));
        stats_->SetMagicDefence(100 + static_cast<u32>(150 * difficultyMul));
        break;

    case EnemyType::DarkWizardBasic:
        stats_->SetMaxHealth(275 + static_cast<u32>(135 * difficultyMul));
        stats_->SetMoveSpeed(33.5f);
        stats_->SetMeleeAttack(65 + static_cast<u32>(35 * difficultyMul));
        stats_->SetMagicAttack(90 + static_cast<u32>(48 * difficultyMul));
        stats_->SetMeleeDefence(20 + static_cast<u32>(20 * difficultyMul));
        stats_->SetMagicDefence(90 + static_cast<u32>(125 * difficultyMul));
        break;
    }

    stats_->SetHealth(stats_->GetMaxHealth());
    SetStats(stats_.get());
}


void BasicEnemy::ResetStats()
{
    ResetStats(Game::Get().GetDirector().GetCurrentDifficultyMultiplier());
}


float BasicEnemy::GetAggroDistance() const
{
    switch (enemyType_) {
    default:
        return 0.0f;

    case EnemyType::SkeletonBasic:
        return 90.0f;

    case EnemyType::AncientWizardBasic:
    case EnemyType::DarkWizardBasic:
        return 120.0f;

    case EnemyType::MagicFlameBasic:
        return 58.0f;

    case EnemyType::GhostBasic:
        return 200.0f;
    }
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

            if (randNum <= 50) {
                weapon = std::make_unique<MagicWeapon>(MagicWeaponType::FlameStaff);
            }
            else if (randNum <= 75) {
                weapon = std::make_unique<MagicWeapon>(MagicWeaponType::WaveStaff);
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

    case EnemyType::AncientWizardBasic:
        // potions
        if (Helper::GenerateRandomBool(1 / 7.0f)) {
            itemsToDrop.emplace_back(std::make_unique<PotionItem>(ItemType::HealthPotion, 2));
        }
        if (Helper::GenerateRandomBool(1 / 7.0f)) {
            itemsToDrop.emplace_back(std::make_unique<PotionItem>(ItemType::MagicPotion, 2));
        }

        // roll for magic weapon
        if (Helper::GenerateRandomBool(1 / 10.0f)) {
            std::unique_ptr<MagicWeapon> weapon;

            auto randNum = Helper::GenerateRandomInt(1, 100);

            if (randNum <= 10) {
                // rare drop!
                weapon = std::make_unique<MagicWeapon>(MagicWeaponType::InvincibilityStaff);
            }
            else if (randNum <= 70) {
                weapon = std::make_unique<MagicWeapon>(MagicWeaponType::WaveStaff);
            }
            else if (randNum <= 85) {
                weapon = std::make_unique<MagicWeapon>(MagicWeaponType::FlameStaff);
            }
            else {
                weapon = std::make_unique<MagicWeapon>(MagicWeaponType::DrainStaff);
            }

            weapon->SetDifficultyMultiplier(Game::Get().GetDirector().GetCurrentDifficultyMultiplier());
            itemsToDrop.emplace_back(std::move(weapon));
        }

        // roll for magic armour
        if (Helper::GenerateRandomBool(1 / 10.0f)) {
            std::unique_ptr<Armour> armour;

            if (Helper::GenerateRandomBool(1 / 3.0f)) {
                armour = std::make_unique<Armour>(ArmourType::BalanceHeadgear);
            }
            else {
                armour = std::make_unique<Armour>(ArmourType::AntiMagicVisor);
            }
            
            armour->SetDifficultyMultiplier(Game::Get().GetDirector().GetCurrentDifficultyMultiplier());
            itemsToDrop.emplace_back(std::move(armour));
        }
        break;

    case EnemyType::DarkWizardBasic:
        // potions
        if (Helper::GenerateRandomBool(1 / 7.0f)) {
            itemsToDrop.emplace_back(std::make_unique<PotionItem>(ItemType::HealthPotion, 2));
        }
        if (Helper::GenerateRandomBool(1 / 7.0f)) {
            itemsToDrop.emplace_back(std::make_unique<PotionItem>(ItemType::MagicPotion, 2));
        }

        // roll for magic weapon
        if (Helper::GenerateRandomBool(1 / 9.0f)) {
            std::unique_ptr<MagicWeapon> weapon;

            auto randNum = Helper::GenerateRandomInt(1, 100);

            if (randNum <= 15) {
                // rare drop!
                weapon = std::make_unique<MagicWeapon>(MagicWeaponType::InvincibilityStaff);
            }
            else if (randNum <= 70) {
                weapon = std::make_unique<MagicWeapon>(MagicWeaponType::WaveStaff);
            }
            else if (randNum <= 85) {
                weapon = std::make_unique<MagicWeapon>(MagicWeaponType::FlameStaff);
            }
            else {
                weapon = std::make_unique<MagicWeapon>(MagicWeaponType::DrainStaff);
            }

            weapon->SetDifficultyMultiplier(Game::Get().GetDirector().GetCurrentDifficultyMultiplier());
            itemsToDrop.emplace_back(std::move(weapon));
        }

        // roll for magic armour
        if (Helper::GenerateRandomBool(1 / 9.0f)) {
            std::unique_ptr<Armour> armour;

            if (Helper::GenerateRandomBool(1 / 2.5f)) {
                armour = std::make_unique<Armour>(ArmourType::BalanceHeadgear);
            }
            else {
                armour = std::make_unique<Armour>(ArmourType::AntiMagicVisor);
            }

            armour->SetDifficultyMultiplier(Game::Get().GetDirector().GetCurrentDifficultyMultiplier());
            itemsToDrop.emplace_back(std::move(armour));
        }
        break;
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
                moveDir_ = Helper::GetUnitVector(playerAggro->GetCenterPosition() - GetCenterPosition());

                // chance to shoot player if aggro'd on them
                if (enemyType_ == EnemyType::AncientWizardBasic &&
                    Helper::GenerateRandomBool(0.3125f * Game::FrameTimeStep.asSeconds())) {
                    auto projectileDir = moveDir_ + sf::Vector2f(Helper::GenerateRandomReal(-0.2f, 0.2f),
                        Helper::GenerateRandomReal(-0.2f, 0.2f));

                    auto projectile = area->GetEntity<ProjectileEntity>(
                        area->EmplaceEntity<ProjectileEntity>(ProjectileType::EnemyMagicWave, projectileDir));
                    assert(projectile);

                    // push player and fire projectile
                    projectile->SetCenterPosition(GetCenterPosition() + projectileDir * 8.0f);
                    projectile->SetDamage(Helper::GenerateRandomInt<u32>(0, stats->GetMagicAttack()));

                    GameAssets::Get().waveSound.play();
                }
                else if (enemyType_ == EnemyType::DarkWizardBasic &&
                    Helper::GenerateRandomBool(0.215f * Game::FrameTimeStep.asSeconds())) {

                    auto damageEffect = area->GetEntity<DamageEffectEntity>(area->EmplaceEntity<DamageEffectEntity>(
                        DamageEffectType::EnemyBlackFlame, sf::seconds(0.5f)));
                    damageEffect->SetCenterPosition(playerAggro->GetCenterPosition());

                    // push player and damage them
                    playerAggro->Attack(Helper::GenerateRandomInt<u32>(0, stats->GetMagicAttack()), DamageType::Magic);
                    playerAggro->MoveWithCollision(sf::Vector2f(Helper::GenerateRandomReal(-8.0f, 8.0f),
                        Helper::GenerateRandomReal(-8.0f, 8.0f)));

                    GameAssets::Get().blastSound.play();
                }
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

            // tick anim if moving or if certain enemy type
            if (moveDir_.x != 0.0f || moveDir_.y != 0.0f ||
                enemyType_ == EnemyType::MagicFlameBasic ||
                enemyType_ == EnemyType::GhostBasic) {
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
            GameAssets::Get().deathSound.play();
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
            // don't bother rendering us if we're dead
            // and in map mode
            if (Game::Get().IsInMapMode()) {
                return;
            }

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

            case EnemyType::AncientWizardBasic:
                enemySprite.setTextureRect(sf::IntRect(32, 112, 16, 16));
                break;

            case EnemyType::DarkWizardBasic:
                enemySprite.setTextureRect(sf::IntRect(32, 128, 16, 16));
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

    case EnemyType::AncientWizardBasic:
        return "Ancient Wizard";

    case EnemyType::DarkWizardBasic:
        return "Dark Wizard";
    }
}