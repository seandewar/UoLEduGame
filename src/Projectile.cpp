#include "Projectile.h"

#include <cmath>

#include "Game.h"
#include "Enemy.h"
#include "Helper.h"


ProjectileEntity::ProjectileEntity(ProjectileType projectileType, const sf::Vector2f& dir) :
UnitEntity(),
projectileType_(projectileType),
attack_(0),
timeLeft_(sf::seconds(2.0f))
{
    SetSize(sf::Vector2f(14.0f, 14.0f));

    float speed = 1.0f;

    switch (projectileType_) {
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
    }
}


bool ProjectileEntity::IsPlayerProjectile() const
{
    switch (projectileType_) {
    default:
        return false;

    case ProjectileType::PlayerMagicWave:
        return true;
    }
}


bool ProjectileEntity::CollidesWithTiles() const
{
    switch (projectileType_) {
    default:
    case ProjectileType::PlayerMagicWave:
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

            switch (projectileType_) {
            default:
            case ProjectileType::PlayerMagicWave:
                isCollision = true;
                hitEnt->Attack(GetAttack(), DamageType::Magic);

                // hit effect
                auto effectEnt = hitEnt->GetAssignedArea()->GetEntity<DamageEffectEntity>(
                    hitEnt->GetAssignedArea()->EmplaceEntity<DamageEffectEntity>(DamageEffectType::Wave,
                    sf::seconds(0.5f)));
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
        projectileSprite->setRotation(Helper::RadiansToDegrees(atan2f(velo_.y, velo_.x)));

        area->AddFrameUIRenderable(projectileSprite);
    }
}