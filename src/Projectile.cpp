#include "Projectile.h"

#include <cmath>

#include "Game.h"
#include "Enemy.h"
#include "Helper.h"


ProjectileEntity::ProjectileEntity(ProjectileType projectileType, const sf::Vector2f& dir) :
UnitEntity(),
projectileType_(projectileType),
damage_(0),
timeLeft_(sf::seconds(2.0f))
{
    SetSize(sf::Vector2f(14.0f, 14.0f));

    float speed = 1.0f;

    switch (projectileType_) {
    case ProjectileType::EnemyMagicFlame:
        SetSize(sf::Vector2f(18.0f, 18.0f));
        speed = 185.0f;
        break;

    case ProjectileType::EffectOrb:
        speed = 80.0f;
        timeLeft_ = sf::seconds(6.0f);
        break;

    case ProjectileType::EnemySmoke:
        speed = 150.0f;
        timeLeft_ = sf::seconds(3.0f);
        break;

    case ProjectileType::EnemyMagicWave:
        speed = 125.0f;
        break;

    case ProjectileType::PlayerMagicWave:
        speed = 200.0f;
        break;
    }

    velo_ = dir * speed;

    SetupAnimations();
}


ProjectileEntity::~ProjectileEntity()
{
}


void ProjectileEntity::SetupAnimations()
{
    switch (projectileType_) {
    case ProjectileType::PlayerMagicWave:
        anim_.AddFrame(sf::Sprite(GameAssets::Get().projectileSpriteSheet, sf::IntRect(0, 0, 16, 16)), sf::seconds(0.1f));
        anim_.AddFrame(sf::Sprite(GameAssets::Get().projectileSpriteSheet, sf::IntRect(16, 0, 16, 16)), sf::seconds(0.1f));
        anim_.AddFrame(sf::Sprite(GameAssets::Get().projectileSpriteSheet, sf::IntRect(32, 0, 16, 16)), sf::seconds(0.1f));
        anim_.AddFrame(sf::Sprite(GameAssets::Get().projectileSpriteSheet, sf::IntRect(48, 0, 16, 16)), sf::seconds(0.1f));
        break;

    case ProjectileType::EnemyMagicWave:
        anim_.AddFrame(sf::Sprite(GameAssets::Get().projectileSpriteSheet, sf::IntRect(0, 16, 16, 16)), sf::seconds(0.1f));
        anim_.AddFrame(sf::Sprite(GameAssets::Get().projectileSpriteSheet, sf::IntRect(16, 16, 16, 16)), sf::seconds(0.1f));
        anim_.AddFrame(sf::Sprite(GameAssets::Get().projectileSpriteSheet, sf::IntRect(32, 16, 16, 16)), sf::seconds(0.1f));
        anim_.AddFrame(sf::Sprite(GameAssets::Get().projectileSpriteSheet, sf::IntRect(48, 16, 16, 16)), sf::seconds(0.1f));
        break;

    case ProjectileType::EnemyMagicFlame:
        anim_.AddFrame(sf::Sprite(GameAssets::Get().projectileSpriteSheet, sf::IntRect(0, 32, 16, 16)), sf::seconds(0.2f));
        anim_.AddFrame(sf::Sprite(GameAssets::Get().projectileSpriteSheet, sf::IntRect(16, 32, 16, 16)), sf::seconds(0.2f));
        anim_.AddFrame(sf::Sprite(GameAssets::Get().projectileSpriteSheet, sf::IntRect(32, 32, 16, 16)), sf::seconds(0.2f));
        anim_.AddFrame(sf::Sprite(GameAssets::Get().projectileSpriteSheet, sf::IntRect(48, 32, 16, 16)), sf::seconds(0.2f));
        break;

    case ProjectileType::EnemySmoke:
        anim_.AddFrame(sf::Sprite(GameAssets::Get().projectileSpriteSheet, sf::IntRect(0, 48, 16, 16)), sf::seconds(0.2f));
        anim_.AddFrame(sf::Sprite(GameAssets::Get().projectileSpriteSheet, sf::IntRect(16, 48, 16, 16)), sf::seconds(0.2f));
        anim_.AddFrame(sf::Sprite(GameAssets::Get().projectileSpriteSheet, sf::IntRect(32, 48, 16, 16)), sf::seconds(0.2f));
        anim_.AddFrame(sf::Sprite(GameAssets::Get().projectileSpriteSheet, sf::IntRect(48, 48, 16, 16)), sf::seconds(0.2f));
        break;

    case ProjectileType::EffectOrb:
        anim_.AddFrame(sf::Sprite(GameAssets::Get().projectileSpriteSheet, sf::IntRect(0, 64, 16, 16)), sf::seconds(0.4f));
        anim_.AddFrame(sf::Sprite(GameAssets::Get().projectileSpriteSheet, sf::IntRect(16, 64, 16, 16)), sf::seconds(0.4f));
        anim_.AddFrame(sf::Sprite(GameAssets::Get().projectileSpriteSheet, sf::IntRect(32, 64, 16, 16)), sf::seconds(0.4f));
        anim_.AddFrame(sf::Sprite(GameAssets::Get().projectileSpriteSheet, sf::IntRect(48, 64, 16, 16)), sf::seconds(0.4f));
        break;
    }
}


bool ProjectileEntity::IsPlayerProjectile() const
{
    switch (projectileType_) {
    default:
    case ProjectileType::EffectOrb:
    case ProjectileType::EnemyMagicWave:
    case ProjectileType::EnemyMagicFlame:
    case ProjectileType::EnemySmoke:
        return false;

    case ProjectileType::PlayerMagicWave:
        return true;
    }
}


bool ProjectileEntity::CollidesWithTiles() const
{
    switch (projectileType_) {
    default:
    case ProjectileType::EffectOrb:
    case ProjectileType::PlayerMagicWave:
    case ProjectileType::EnemyMagicWave:
    case ProjectileType::EnemyMagicFlame:
    case ProjectileType::EnemySmoke:
        return false;
    }
}


void ProjectileEntity::Tick()
{
    auto area = GetAssignedArea();

    if (!area) {
        return;
    }

    // tile collision
    bool isCollision = false;
    
    if (CollidesWithTiles()) {
        isCollision = !MoveWithCollision(velo_ * Game::FrameTimeStep.asSeconds());
    }
    else {
        Move(velo_ * Game::FrameTimeStep.asSeconds());
    }

    // ent collision
    if (projectileType_ != ProjectileType::EffectOrb) {
        std::vector<EntityId> hitEntIds;

        if (IsPlayerProjectile()) {
            hitEntIds = area->GetAllWorldEntsInRectangle<Enemy>(GetRectangle());
        }
        else {
            hitEntIds = area->GetAllWorldEntsInRectangle<PlayerEntity>(GetRectangle());
        }

        for (auto eId : hitEntIds) {
            auto hitEnt = area->GetEntity<AliveEntity>(eId);
            assert(hitEnt);

            if (hitEnt->GetStats() && hitEnt->GetStats()->IsAlive()) {
                // projectile type effect
                isCollision = true;

                // hit type
                switch (projectileType_) {
                case ProjectileType::PlayerMagicWave:
                case ProjectileType::EnemyMagicWave:
                case ProjectileType::EnemyMagicFlame:
                    hitEnt->Attack(GetDamage(), DamageType::Magic);
                    break;

                case ProjectileType::EnemySmoke:
                    hitEnt->Attack(GetDamage(), DamageType::Other);
                    break;
                }

                // hit effect
                DamageEffectType effectType;

                switch (projectileType_) {
                case ProjectileType::PlayerMagicWave:
                    effectType = DamageEffectType::PlayerWave;
                    break;

                case ProjectileType::EnemyMagicWave:
                    effectType = DamageEffectType::EnemyWave;
                    break;

                case ProjectileType::EnemyMagicFlame:
                    effectType = DamageEffectType::EnemyMagicFlame;
                    break;

                case ProjectileType::EnemySmoke:
                    effectType = DamageEffectType::EnemySmoke;
                    break;
                }

                auto effectEnt = hitEnt->GetAssignedArea()->GetEntity<DamageEffectEntity>(
                    hitEnt->GetAssignedArea()->EmplaceEntity<DamageEffectEntity>(effectType, sf::seconds(0.5f)));
                effectEnt->SetCenterPosition(hitEnt->GetCenterPosition());

                // push ent
                hitEnt->MoveWithCollision(0.05f * velo_);
                break;
            }
        }
    }

    // remove on collision or expire
    if (isCollision || timeLeft_ <= sf::Time::Zero) {
        MarkForDeletion();
    }

    timeLeft_ -= Game::FrameTimeStep;
    anim_.Tick();
}


void ProjectileEntity::Render(sf::RenderTarget& target)
{
    auto area = GetAssignedArea();

    if (area) {
        auto projectileSprite = std::make_unique<sf::Sprite>(anim_.GetCurrentFrame());
        projectileSprite->setPosition(GetPosition() + GetSize() * 0.5f);
        projectileSprite->setOrigin(GetSize() * 0.5f);

        switch (projectileType_) {
        case ProjectileType::PlayerMagicWave:
        case ProjectileType::EnemyMagicWave:
            projectileSprite->setRotation(Helper::RadiansToDegrees(atan2f(velo_.y, velo_.x)));
            break;

        case ProjectileType::EnemyMagicFlame:
            projectileSprite->setScale(1.5f, 1.5f);
            break;

        case ProjectileType::EffectOrb:
            projectileSprite->setColor(sf::Color(
                Helper::GenerateRandomInt(0, 255),
                Helper::GenerateRandomInt(0, 255),
                Helper::GenerateRandomInt(0, 255)));
            break;
        }

        area->AddFrameUIRenderable(projectileSprite);
    }
}