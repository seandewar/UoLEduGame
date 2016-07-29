#include "Entity.h"

#include <SFML/Graphics/Text.hpp>
#include <SFML/Graphics/Sprite.hpp>

#include "World.h"
#include "Game.h"
#include "Helper.h"


Entity::Entity() :
assignedId_(Entity::InvalidId),
assignedArea_(nullptr),
markedForDeletion_(false)
{
}


Entity::~Entity()
{
}


WorldEntity::WorldEntity() :
Entity()
{
}


WorldEntity::~WorldEntity()
{
}


void WorldEntity::MoveWithCollision(const sf::Vector2f& d)
{
    auto area = GetAssignedArea();
    if (area) {
        sf::Vector2f newPos;
        area->TryCollisionRectMove(GetRectangle(), d, &newPos);

        SetPosition(newPos);
    }
}


UnitEntity::UnitEntity() :
WorldEntity()
{
}


UnitEntity::~UnitEntity()
{
}


DamageTextEntity::DamageTextEntity(DamageType type, u32 damageAmount, const sf::Vector2f& velo,
    const sf::Time& displayTime) :
type_(type),
damage_(damageAmount),
velo_(velo),
timeLeft_(displayTime)
{
    SetSize(sf::Vector2f(16.0f, 5.0f));
}


DamageTextEntity::~DamageTextEntity()
{
}


void DamageTextEntity::Tick()
{
    if (IsMarkedForDeletion()) {
        return;
    }

    SetPosition(GetPosition() + velo_ * Game::FrameTimeStep.asSeconds());

    if (timeLeft_ > sf::Time::Zero) {
        timeLeft_ -= Game::FrameTimeStep;
    }
    else {
        MarkForDeletion();
        return;
    }
}


void DamageTextEntity::Render(sf::RenderTarget& target)
{
    auto area = GetAssignedArea();

    if (!area) {
        return;
    }

    // damage type sprite
    auto damageTypeSprite = std::make_unique<sf::Sprite>(GameAssets::Get().damageTypesSpriteSheet);

    switch (type_) {
    case DamageType::Melee:
        damageTypeSprite->setTextureRect(sf::IntRect(0, 0, 16, 16));
        break;

    case DamageType::Magic:
        damageTypeSprite->setTextureRect(sf::IntRect(16, 0, 16, 16));
        break;

    default:
    case DamageType::Other:
        damageTypeSprite->setTextureRect(sf::IntRect(0, 16, 16, 16));
        break;
    }

    damageTypeSprite->setScale(0.35f, 0.35f);
    damageTypeSprite->setPosition(GetPosition() - sf::Vector2f(0.0f, 2.0f));

    area->AddFrameUIRenderable(damageTypeSprite);

    // damage amount text / blocked sprite
    if (damage_ > 0) {
        auto damageText = std::make_unique<sf::Text>(std::to_string(damage_), GameAssets::Get().gameFont, 12);
        damageText->setScale(0.25f, 0.25f);
        damageText->setPosition(GetPosition() + sf::Vector2f(6.0f, 0.0f));

        auto damageShadow = Helper::GetTextDropShadow(*damageText, sf::Vector2f(0.35f, 0.35f));

        area->AddFrameUIRenderable(damageShadow);
        area->AddFrameUIRenderable(damageText);
    }
    else {
        auto damageBlockedSprite = std::make_unique<sf::Sprite>(GameAssets::Get().damageTypesSpriteSheet,
            sf::IntRect(16, 16, 16, 16));
        damageBlockedSprite->setScale(0.35f, 0.35f);
        damageBlockedSprite->setPosition(GetPosition() + sf::Vector2f(6.0f, -2.0f));

        area->AddFrameUIRenderable(damageBlockedSprite);
    }
}


AliveStats::AliveStats() :
maxHealth_(1),
health_(1),
maxMana_(1),
mana_(1),
meleeAttack_(1),
magicAttack_(1),
meleeDefence_(0),
magicDefence_(0),
moveSpeed_(45.0f)
{
}


AliveStats::~AliveStats()
{
}


AliveEntity::AliveEntity() :
UnitEntity()
{
}


AliveEntity::~AliveEntity()
{
}


void AliveEntity::Attack(u32 initialDamage, DamageType source)
{
    auto stats = GetStats();

    if (stats && stats->IsAlive()) {
        switch (source) {
        case DamageType::Melee:
            Damage(static_cast<u32>(std::max<i64>(0, static_cast<i64>(initialDamage) - 
                Helper::GenerateRandomInt<u32>(0, stats->GetMeleeDefence()))), source);
            break;

        case DamageType::Magic:
            Damage(static_cast<u32>(std::max<i64>(0, static_cast<i64>(initialDamage) -
                Helper::GenerateRandomInt<u32>(0, stats->GetMagicDefence()))), source);
            break;

        default:
        case DamageType::Other:
            Damage(initialDamage, source);
            break;
        }
    }
}


void AliveEntity::Damage(u32 amount, DamageType source)
{
    auto stats = GetStats();

    if (stats && stats->IsAlive()) {
        stats->ApplyDamage(amount);

        auto damageText = GetAssignedArea()->GetEntity<DamageTextEntity>(
            GetAssignedArea()->EmplaceEntity<DamageTextEntity>(source, amount));
        damageText->SetCenterPosition(GetCenterPosition() + sf::Vector2f(0.0f, 4.0f));
    }
}


void AliveEntity::Heal(u32 amount)
{
    auto stats = GetStats();

    if (stats && stats->IsAlive()) {
        stats->ApplyHealing(amount);
    }
}