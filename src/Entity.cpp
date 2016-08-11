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


bool WorldEntity::MoveWithCollision(const sf::Vector2f& d)
{
    bool noCollision = true;
    auto area = GetAssignedArea();

    if (area) {
        sf::Vector2f newPos;
        noCollision = area->TryCollisionRectMove(GetRectangle(), d, &newPos);

        SetPosition(newPos);
    }

    return noCollision;
}


UnitEntity::UnitEntity() :
WorldEntity()
{
}


UnitEntity::~UnitEntity()
{
}


DamageTextEntity::DamageTextEntity(DamageType type, u32 damageAmount, const sf::Color& color, const sf::Vector2f& velo,
    const sf::Time& displayTime) :
type_(type),
damage_(damageAmount),
velo_(velo),
color_(color),
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
    if (damage_ > 0 || type_ == DamageType::Other) {
        auto damageText = std::make_unique<sf::Text>(std::to_string(damage_), GameAssets::Get().gameFont, 12);
        damageText->setScale(0.25f, 0.25f);
        damageText->setPosition(GetPosition() + sf::Vector2f(6.0f, 0.0f));
        damageText->setColor(color_);

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


DamageEffectEntity::DamageEffectEntity(DamageEffectType type, const sf::Time& duration) :
effectType_(type),
timeLeft_(duration)
{
    SetSize(sf::Vector2f(16.0f, 16.0f));
    SetupAnimations();
}


DamageEffectEntity::~DamageEffectEntity()
{
}


void DamageEffectEntity::SetupAnimations()
{
    switch (effectType_) {
    case DamageEffectType::Zero:
        anim_.AddFrame(sf::Sprite(GameAssets::Get().effectSpriteSheet, sf::IntRect(0, 0, 16, 16)));
        anim_.AddFrame(sf::Sprite(GameAssets::Get().effectSpriteSheet, sf::IntRect(16, 0, 16, 16)));
        anim_.AddFrame(sf::Sprite(GameAssets::Get().effectSpriteSheet, sf::IntRect(32, 0, 16, 16)));
        anim_.AddFrame(sf::Sprite(GameAssets::Get().effectSpriteSheet, sf::IntRect(48, 0, 16, 16)));
        break;

    case DamageEffectType::Flame:
        anim_.AddFrame(sf::Sprite(GameAssets::Get().effectSpriteSheet, sf::IntRect(0, 16, 16, 16)));
        anim_.AddFrame(sf::Sprite(GameAssets::Get().effectSpriteSheet, sf::IntRect(16, 16, 16, 16)));
        anim_.AddFrame(sf::Sprite(GameAssets::Get().effectSpriteSheet, sf::IntRect(32, 16, 16, 16)));
        anim_.AddFrame(sf::Sprite(GameAssets::Get().effectSpriteSheet, sf::IntRect(48, 16, 16, 16)));
        break;

    case DamageEffectType::Drain:
        anim_.AddFrame(sf::Sprite(GameAssets::Get().effectSpriteSheet, sf::IntRect(0, 32, 16, 16)));
        anim_.AddFrame(sf::Sprite(GameAssets::Get().effectSpriteSheet, sf::IntRect(16, 32, 16, 16)));
        anim_.AddFrame(sf::Sprite(GameAssets::Get().effectSpriteSheet, sf::IntRect(32, 32, 16, 16)));
        anim_.AddFrame(sf::Sprite(GameAssets::Get().effectSpriteSheet, sf::IntRect(48, 32, 16, 16)));
        break;

    case DamageEffectType::Wave:
        anim_.AddFrame(sf::Sprite(GameAssets::Get().effectSpriteSheet, sf::IntRect(0, 48, 16, 16)));
        anim_.AddFrame(sf::Sprite(GameAssets::Get().effectSpriteSheet, sf::IntRect(16, 48, 16, 16)));
        anim_.AddFrame(sf::Sprite(GameAssets::Get().effectSpriteSheet, sf::IntRect(32, 48, 16, 16)));
        anim_.AddFrame(sf::Sprite(GameAssets::Get().effectSpriteSheet, sf::IntRect(48, 48, 16, 16)));
        break;
    }
}


void DamageEffectEntity::Tick()
{
    if (timeLeft_ > sf::Time::Zero) {
        timeLeft_ -= Game::FrameTimeStep;
        anim_.Tick();
    }
    else {
        MarkForDeletion();
        return;
    }
}


void DamageEffectEntity::Render(sf::RenderTarget& target)
{
    auto effectSprite = std::make_unique<sf::Sprite>(anim_.GetCurrentFrame());
    effectSprite->setPosition(GetPosition());

    GetAssignedArea()->AddFrameUIRenderable(effectSprite);
}


AliveStats::AliveStats() :
maxHealth_(1),
health_(1),
maxMana_(1),
mana_(1),
meleeAttack_(0),
magicAttack_(0),
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


u32 AliveEntity::Attack(u32 initialDamage, DamageType source)
{
    auto stats = GetStats();

    if (stats && stats->IsAlive()) {
        switch (source) {
        case DamageType::Melee:
            return Damage(static_cast<u32>(std::max<i64>(0, static_cast<i64>(initialDamage) - 
                Helper::GenerateRandomInt<u32>(0, stats->GetMeleeDefence()))), source);

        case DamageType::Magic:
            return Damage(static_cast<u32>(std::max<i64>(0, static_cast<i64>(initialDamage) -
                Helper::GenerateRandomInt<u32>(0, stats->GetMagicDefence()))), source);

        default:
        case DamageType::Other:
            return Damage(initialDamage, source);
        }
    }

    return 0;
}


u32 AliveEntity::Damage(u32 amount, DamageType source)
{
    auto stats = GetStats();

    if (stats && stats->IsAlive()) {
        stats->ApplyDamage(amount);

        auto damageText = GetAssignedArea()->GetEntity<DamageTextEntity>(
            GetAssignedArea()->EmplaceEntity<DamageTextEntity>(source, amount, sf::Color(255, 75, 75)));
        damageText->SetCenterPosition(GetCenterPosition() + sf::Vector2f(0.0f, 4.0f));
    }
    else {
        return 0;
    }

    if (amount <= 0) {
        GameAssets::Get().blockSound.play();
    }
    else if (source == DamageType::Other) {
        GameAssets::Get().armourPenSound.play();
    }

    return amount;
}


void AliveEntity::Heal(u32 amount)
{
    auto stats = GetStats();

    if (stats && stats->IsAlive()) {
        stats->ApplyHealing(amount);
    }
}