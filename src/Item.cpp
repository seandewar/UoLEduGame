#include "Item.h"

#include "Game.h"
#include "Player.h"
#include "Enemy.h"
#include "Helper.h"
#include "Projectile.h"


Item::Item(int maxAmount, int amount)
{
    SetMaxAmount(maxAmount);
    SetAmount(amount);
}


Item::~Item()
{
}


void Item::TickDelayTimeLeft()
{
    if (useDelayLeft_ > sf::Time::Zero) {
        useDelayLeft_ -= Game::FrameTimeStep;
    }
}


ArtefactItem::ArtefactItem(ArtefactType artefactType) :
Item(1, 1),
artefactType_(artefactType)
{
}


ArtefactItem::~ArtefactItem()
{
}


sf::Sprite ArtefactItem::GetSprite() const
{
    sf::Sprite artefactSprite(GameAssets::Get().itemsSpriteSheet);

    switch (artefactType_) {
    case ArtefactType::Appearance1:
        artefactSprite.setTextureRect(sf::IntRect(0, 32, 16, 16));
        break;

    case ArtefactType::Appearance2:
        artefactSprite.setTextureRect(sf::IntRect(0, 48, 16, 16));
        break;

    case ArtefactType::Appearance3:
        artefactSprite.setTextureRect(sf::IntRect(0, 64, 16, 16));
        break;

    case ArtefactType::Appearance4:
        artefactSprite.setTextureRect(sf::IntRect(0, 80, 16, 16));
        break;

    case ArtefactType::Appearance5:
        artefactSprite.setTextureRect(sf::IntRect(0, 96, 16, 16));
        break;

    default:
        return sf::Sprite();
    }

    return artefactSprite;
}


PotionItem::PotionItem(ItemType potionType, int amount) :
Item(10, amount),
potionType_(potionType)
{
    if (potionType_ != ItemType::HealthPotion &&
        potionType_ != ItemType::MagicPotion) {
        throw std::runtime_error("Bad potion ItemType!");
    }
}


PotionItem::~PotionItem()
{
}


void PotionItem::Use(PlayerEntity* player)
{
    if (!player || !player->GetStats() || GetAmount() <= 0) {
        return;
    }

    auto stats = player->GetStats();
    bool drankPotion = false;

    switch (potionType_) {
    case ItemType::HealthPotion:
        if (stats->GetHealth() < stats->GetMaxHealth()) {
            player->Heal(200);
            drankPotion = true;

            Game::Get().AddMessage("You drink a Health Potion.", sf::Color(255, 100, 100));
        }
        else {
            Game::Get().AddMessage("You are already at full Health.");
        }
        break;

    case ItemType::MagicPotion:
        if (stats->GetMana() < stats->GetMaxMana()) {
            stats->SetMana(std::min(stats->GetMaxMana(), stats->GetMana() + 250));
            drankPotion = true;

            Game::Get().AddMessage("You drink a Magic Potion.", sf::Color(100, 100, 255));
        }
        else {
            Game::Get().AddMessage("You are already have full Mana.");
        }
        break;
    }

    if (drankPotion) {
        RemoveAmount(1);
        GameAssets::Get().drinkSound.play();
    }
}


sf::Sprite PotionItem::GetSprite() const
{
    sf::Sprite potionSprite(GameAssets::Get().itemsSpriteSheet);

    switch (potionType_) {
    case ItemType::HealthPotion:
        potionSprite.setTextureRect(sf::IntRect(0, 0, 16, 16));
        break;

    case ItemType::MagicPotion:
        potionSprite.setTextureRect(sf::IntRect(0, 16, 16, 16));
        break;

    default:
        return sf::Sprite();
    }

    return potionSprite;
}


std::string PotionItem::GetItemName() const
{
    switch (potionType_) {
    case ItemType::HealthPotion:
        return "Health Potion";

    case ItemType::MagicPotion:
        return "Magic Potion";

    default:
        return "Potion";
    }
}


Armour::Armour(ArmourType armourType) :
Item(1, 1),
armourType_(armourType),
difficultyMul_(0.0f)
{
}


Armour::~Armour()
{
}


std::string Armour::GetItemName() const
{
    std::string itemName;

    switch (armourType_) {
    default:
        return std::string();

    case ArmourType::WarriorHelmet:
        itemName = "Rock Helmet";
        break;

    case ArmourType::AntiMagicVisor:
        itemName = "Anti-Magic Visor";
        break;

    case ArmourType::BalanceHeadgear:
        itemName = "Balancer";
        break;
    }

    // append difficulty num
    if (difficultyMul_ > 0.0f) {
        std::ostringstream oss;
        oss << itemName << " " << std::fixed << std::setprecision(1) << difficultyMul_;
        return oss.str();
    }
    else {
        return itemName;
    }
}


u32 Armour::GetMeleeDefense() const
{
    switch (armourType_) {
    default:
        return 0;

    case ArmourType::WarriorHelmet:
        return 30 + static_cast<u32>(66 * difficultyMul_);

    case ArmourType::AntiMagicVisor:
        return 11 + static_cast<u32>(21 * difficultyMul_);

    case ArmourType::BalanceHeadgear:
        return 22 + static_cast<u32>(45 * difficultyMul_);
    }
}


u32 Armour::GetMagicDefense() const
{
    switch (armourType_) {
    default:
        return 0;

    case ArmourType::WarriorHelmet:
        return 11 + static_cast<u32>(21 * difficultyMul_);

    case ArmourType::AntiMagicVisor:
        return 30 + static_cast<u32>(66 * difficultyMul_);

    case ArmourType::BalanceHeadgear:
        return 22 + static_cast<u32>(45 * difficultyMul_);
    }
}


std::string Armour::GetShortDescription() const
{
    switch (armourType_) {
    default:
        return std::string();

    case ArmourType::WarriorHelmet:
        return "Great melee defence";

    case ArmourType::AntiMagicVisor:
        return "Great magic defence";

    case ArmourType::BalanceHeadgear:
        return "Magic and melee defence";
    }
}


sf::Sprite Armour::GetSprite() const
{
    sf::Sprite armourItemSprite(GameAssets::Get().itemsSpriteSheet);

    switch (armourType_) {
    default:
        return sf::Sprite();

    case ArmourType::WarriorHelmet:
        armourItemSprite.setTextureRect(sf::IntRect(16, 32, 16, 16));
        break;

    case ArmourType::AntiMagicVisor:
        armourItemSprite.setTextureRect(sf::IntRect(32, 32, 16, 16));
        break;

    case ArmourType::BalanceHeadgear:
        armourItemSprite.setTextureRect(sf::IntRect(48, 32, 16, 16));
        break;
    }

    return armourItemSprite;
}


sf::Sprite Armour::GetPlayerSprite(PlayerFacingDirection dir) const
{
    sf::Sprite armourSprite(GameAssets::Get().playerSpriteSheet);

    switch (armourType_) {
    default:
        return sf::Sprite();

    case ArmourType::WarriorHelmet:
        switch (dir) {
        case PlayerFacingDirection::Down:
            armourSprite.setTextureRect(sf::IntRect(64, 0, 16, 16));
            break;

        case PlayerFacingDirection::Up:
            armourSprite.setTextureRect(sf::IntRect(80, 0, 16, 16));
            break;

        case PlayerFacingDirection::Left:
            armourSprite.setTextureRect(sf::IntRect(96, 0, 16, 16));
            break;

        case PlayerFacingDirection::Right:
            armourSprite.setTextureRect(sf::IntRect(112, 0, 16, 16));
            break;
        }
        break;

    case ArmourType::AntiMagicVisor:
        switch (dir) {
        case PlayerFacingDirection::Down:
            armourSprite.setTextureRect(sf::IntRect(64, 16, 16, 16));
            break;

        case PlayerFacingDirection::Up:
            armourSprite.setTextureRect(sf::IntRect(80, 16, 16, 16));
            break;

        case PlayerFacingDirection::Left:
            armourSprite.setTextureRect(sf::IntRect(96, 16, 16, 16));
            break;

        case PlayerFacingDirection::Right:
            armourSprite.setTextureRect(sf::IntRect(112, 16, 16, 16));
            break;
        }
        break;

    case ArmourType::BalanceHeadgear:
        switch (dir) {
        case PlayerFacingDirection::Down:
            armourSprite.setTextureRect(sf::IntRect(64, 32, 16, 16));
            break;

        case PlayerFacingDirection::Up:
            armourSprite.setTextureRect(sf::IntRect(80, 32, 16, 16));
            break;

        case PlayerFacingDirection::Left:
            armourSprite.setTextureRect(sf::IntRect(96, 32, 16, 16));
            break;

        case PlayerFacingDirection::Right:
            armourSprite.setTextureRect(sf::IntRect(112, 32, 16, 16));
            break;
        }
        break;
    }

    return armourSprite;
}


BaseWeaponItem::BaseWeaponItem(const std::string& itemName) :
Item(1, 1),
itemName_(itemName),
difficultyMul_(0.0f)
{
}


BaseWeaponItem::~BaseWeaponItem()
{
}


MeleeWeapon::MeleeWeapon(MeleeWeaponType meleeWeaponType) :
meleeWeaponType_(meleeWeaponType)
{
    switch (meleeWeaponType_) {
    case MeleeWeaponType::BasicSword:
        SetItemName("Basic Sword");
        break;

    case MeleeWeaponType::AdventurerSword:
        SetItemName("Adventurers' Sword");
        break;

    case MeleeWeaponType::GraniteBlade:
        SetItemName("Granite Blade");
        break;

    case MeleeWeaponType::RoguesDagger:
        SetItemName("Rogues' Dagger");
        break;

    case MeleeWeaponType::ShardBlade:
        SetItemName("Shard Blade");
        break;

    case MeleeWeaponType::ThornedSabre:
        SetItemName("Thorned Sabre");
        break;

    case MeleeWeaponType::Zeraleth:
        SetItemName("Zeraleth");
        break;

    case MeleeWeaponType::AntiBlobSpear:
        SetItemName("Anti-Blob Spear");
        break;

    case MeleeWeaponType::RegenBlade:
        SetItemName("Regen Blade");
        break;
    }
}


MeleeWeapon::~MeleeWeapon()
{
}


u32 MeleeWeapon::GetAttack() const
{
    switch (meleeWeaponType_) {
    default:
        return 0;

    case MeleeWeaponType::BasicSword:
        return 100;

    case MeleeWeaponType::AdventurerSword:
        return 135 + static_cast<u32>(70 * GetDifficultyMultiplier());

    case MeleeWeaponType::GraniteBlade:
        return 260 + static_cast<u32>(165 * GetDifficultyMultiplier());

    case MeleeWeaponType::RoguesDagger:
        return 70 + static_cast<u32>(48 * GetDifficultyMultiplier());

    case MeleeWeaponType::ShardBlade:
        return 130 + static_cast<u32>(55 * GetDifficultyMultiplier());

    case MeleeWeaponType::ThornedSabre:
        return 150 + static_cast<u32>(45 * GetDifficultyMultiplier());

    case MeleeWeaponType::Zeraleth:
        return 115 + static_cast<u32>(42 * GetDifficultyMultiplier());

    case MeleeWeaponType::AntiBlobSpear:
        return 105 + static_cast<u32>(38 * GetDifficultyMultiplier());

    case MeleeWeaponType::RegenBlade:
        return 125 + static_cast<u32>(62 * GetDifficultyMultiplier());
    }
}


float MeleeWeapon::GetAttackRange() const
{
    switch (meleeWeaponType_) {
    default:
    case MeleeWeaponType::BasicSword:
    case MeleeWeaponType::AdventurerSword:
    case MeleeWeaponType::GraniteBlade:
    case MeleeWeaponType::ShardBlade:
    case MeleeWeaponType::Zeraleth:
    case MeleeWeaponType::RegenBlade:
        return 18.0f;

    case MeleeWeaponType::AntiBlobSpear:
        return 21.0f;

    case MeleeWeaponType::ThornedSabre:
        return 16.0f;

    case MeleeWeaponType::RoguesDagger:
        return 11.0f;
    }
}


sf::Time MeleeWeapon::GetUseDelay() const
{
    switch (meleeWeaponType_) {
    default:
    case MeleeWeaponType::BasicSword:
    case MeleeWeaponType::AdventurerSword:
    case MeleeWeaponType::Zeraleth:
    case MeleeWeaponType::RegenBlade:
    case MeleeWeaponType::ShardBlade:
        return sf::seconds(0.4f);

    case MeleeWeaponType::AntiBlobSpear:
        return sf::seconds(0.345f);

    case MeleeWeaponType::ThornedSabre:
        return sf::seconds(0.425f);

    case MeleeWeaponType::GraniteBlade:
        return sf::seconds(1.15f);

    case MeleeWeaponType::RoguesDagger:
        return sf::seconds(0.25f);
    }
}


std::string MeleeWeapon::GetShortDescription() const
{
    switch (meleeWeaponType_) {
    default:
        return std::string();

    case MeleeWeaponType::BasicSword:
        return "A simple sword";

    case MeleeWeaponType::AdventurerSword:
        return "A versatile sword";

    case MeleeWeaponType::GraniteBlade:
        return "Chance to ignore armour";

    case MeleeWeaponType::RoguesDagger:
        return "Chance to ignore armour";

    case MeleeWeaponType::ShardBlade:
        return "Chance for high magic hits";

    case MeleeWeaponType::ThornedSabre:
        return "Chance to ignore armour";

    case MeleeWeaponType::Zeraleth:
        return "Massive damage to undead";

    case MeleeWeaponType::AntiBlobSpear:
        return "Massive damage to blobs";

    case MeleeWeaponType::RegenBlade:
        return "Restores some mana on hit";
    }
}


void MeleeWeapon::Use(PlayerEntity* player)
{
    if (!player || !player->GetAssignedArea() || GetAmount() <= 0) {
        return;
    }

    // decide hit area
    auto ppos = player->GetPosition();
    auto psize = player->GetSize();
    sf::FloatRect hitRect;

    switch (player->GetFacingDirection()) {
    case PlayerFacingDirection::Up:
        hitRect = sf::FloatRect(ppos.x, ppos.y - GetAttackRange(), psize.x, GetAttackRange());
        break;

    case PlayerFacingDirection::Down:
        hitRect = sf::FloatRect(ppos.x, ppos.y + psize.y, psize.x, GetAttackRange());
        break;

    case PlayerFacingDirection::Left:
        hitRect = sf::FloatRect(ppos.x - GetAttackRange(), ppos.y, GetAttackRange(), psize.y);
        break;

    case PlayerFacingDirection::Right:
        hitRect = sf::FloatRect(ppos.x + psize.x, ppos.y, GetAttackRange(), psize.y);
        break;
    }

    // get hit ents
    auto hitEntIds = player->GetAssignedArea()->GetAllWorldEntsInRectangle<Enemy>(hitRect);

    // decide weapon action
    for (auto id : hitEntIds) {
        auto ent = player->GetAssignedArea()->GetEntity<Enemy>(id);
        assert(ent);

        if (!ent->GetStats() || !ent->GetStats()->IsAlive()) {
            continue;
        }

        // hit sound
        GameAssets::Get().hitSound.play();

        // damage enemy
        switch (meleeWeaponType_) {
        default:
        case MeleeWeaponType::BasicSword:
        case MeleeWeaponType::AdventurerSword:
            ent->Attack(Helper::GenerateRandomInt<u32>(0, GetAttack()), DamageType::Melee);
            break;

        case MeleeWeaponType::RoguesDagger:
            if (Helper::GenerateRandomBool(1 / 5.0f)) {
                ent->Attack(Helper::GenerateRandomInt<u32>(GetAttack() / 2, GetAttack()), DamageType::Other);
            }
            else {
                ent->Attack(Helper::GenerateRandomInt<u32>(0, GetAttack()), DamageType::Melee);
            }
            break;

        case MeleeWeaponType::ShardBlade:
            if (Helper::GenerateRandomBool(1 / 10.0f)) {
                GameAssets::Get().specSound.play();
                ent->Attack(Helper::GenerateRandomInt<u32>(0, GetAttack() * 2), DamageType::Magic);
            }
            else {
                ent->Attack(Helper::GenerateRandomInt<u32>(0, GetAttack()), DamageType::Melee);
            }
            break;

        case MeleeWeaponType::ThornedSabre:
            if (Helper::GenerateRandomBool(1 / 6.0f)) {
                ent->Attack(GetAttack(), DamageType::Other);
            }
            else {
                ent->Attack(Helper::GenerateRandomInt<u32>(0, GetAttack()), DamageType::Melee);
            }
            break;

        case MeleeWeaponType::GraniteBlade:
            if (Helper::GenerateRandomBool(1 / 10.0f)) {
                ent->Attack(Helper::GenerateRandomInt<u32>(GetAttack() / 5, GetAttack()), DamageType::Other);
            }
            else {
                ent->Attack(Helper::GenerateRandomInt<u32>(0, GetAttack()), DamageType::Melee);
            }
            break;

        case MeleeWeaponType::Zeraleth:
            if (ent->GetEnemyType() == EnemyType::GhostBasic ||
                ent->GetEnemyType() == EnemyType::SkeletonBasic) {
                GameAssets::Get().specSound.play();
                ent->Attack(Helper::GenerateRandomInt<u32>(GetAttack() * 2, GetAttack() * 4), DamageType::Other);
            }
            else {
                ent->Attack(Helper::GenerateRandomInt<u32>(0, GetAttack()), DamageType::Melee);
            }
            break;

        case MeleeWeaponType::AntiBlobSpear:
            if (ent->GetEnemyType() == EnemyType::GreenBlobBasic ||
                ent->GetEnemyType() == EnemyType::BlueBlobBasic ||
                ent->GetEnemyType() == EnemyType::RedBlobBasic ||
                ent->GetEnemyType() == EnemyType::PinkBlobBasic) {
                GameAssets::Get().specSound.play();
                ent->Attack(Helper::GenerateRandomInt<u32>(GetAttack() * 4, GetAttack() * 5), DamageType::Other);
            }
            else {
                ent->Attack(Helper::GenerateRandomInt<u32>(0, GetAttack()), DamageType::Melee);
            }
            break;

        case MeleeWeaponType::RegenBlade:
            ent->Attack(Helper::GenerateRandomInt<u32>(0, GetAttack()), DamageType::Melee);
            
            if (player->GetStats()) {
                player->GetStats()->SetMana(std::min(player->GetStats()->GetMaxMana(),
                    player->GetStats()->GetMana() + Helper::GenerateRandomInt(1, 15)));
            }
            break;
        }

        // push enemy back
        switch (player->GetFacingDirection()) {
        case PlayerFacingDirection::Up:
            ent->MoveWithCollision(sf::Vector2f(0.0f, -4.0f));
            break;

        case PlayerFacingDirection::Down:
            ent->MoveWithCollision(sf::Vector2f(0.0f, 4.0f));
            break;

        case PlayerFacingDirection::Left:
            ent->MoveWithCollision(sf::Vector2f(-4.0f, 0.0f));
            break;

        case PlayerFacingDirection::Right:
            ent->MoveWithCollision(sf::Vector2f(4.0f, 0.0f));
            break;
        }
    }

    GameAssets::Get().attackSound.play();
    player->PlayAttackAnimation(PlayerSelectedWeapon::Melee);
}


sf::Sprite MeleeWeapon::GetSprite() const
{
    sf::Sprite weaponSprite(GameAssets::Get().itemsSpriteSheet);

    switch (meleeWeaponType_) {
    case MeleeWeaponType::BasicSword:
        weaponSprite.setTextureRect(sf::IntRect(16, 0, 16, 16));
        break;

    case MeleeWeaponType::AdventurerSword:
        weaponSprite.setTextureRect(sf::IntRect(32, 0, 16, 16));
        break;

    case MeleeWeaponType::GraniteBlade:
        weaponSprite.setTextureRect(sf::IntRect(48, 0, 16, 16));
        break;

    case MeleeWeaponType::RoguesDagger:
        weaponSprite.setTextureRect(sf::IntRect(64, 0, 16, 16));
        break;

    case MeleeWeaponType::ShardBlade:
        weaponSprite.setTextureRect(sf::IntRect(80, 0, 16, 16));
        break;

    case MeleeWeaponType::ThornedSabre:
        weaponSprite.setTextureRect(sf::IntRect(96, 0, 16, 16));
        break;

    case MeleeWeaponType::Zeraleth:
        weaponSprite.setTextureRect(sf::IntRect(112, 0, 16, 16));
        break;

    case MeleeWeaponType::AntiBlobSpear:
        weaponSprite.setTextureRect(sf::IntRect(128, 0, 16, 16));
        break;

    case MeleeWeaponType::RegenBlade:
        weaponSprite.setTextureRect(sf::IntRect(144, 0, 16, 16));
        break;

    default:
        return sf::Sprite();
    }

    return weaponSprite;
}


MagicWeapon::MagicWeapon(MagicWeaponType magicWeaponType) :
magicWeaponType_(magicWeaponType)
{
    switch (magicWeaponType_) {
    case MagicWeaponType::ZeroStaff:
        SetItemName("Zero Staff");
        break;

    case MagicWeaponType::FlameStaff:
        SetItemName("Staff of Flame");
        break;

    case MagicWeaponType::DrainStaff:
        SetItemName("Vampiric Staff");
        break;

    case MagicWeaponType::InvincibilityStaff:
        SetItemName("Staff of Protection");
        break;

    case MagicWeaponType::WaveStaff:
        SetItemName("Wave Staff");
        break;
    }
}


MagicWeapon::~MagicWeapon()
{
}


u32 MagicWeapon::GetAttack() const
{
    switch (magicWeaponType_) {
    default:
    case MagicWeaponType::InvincibilityStaff:
        return 0;

    case MagicWeaponType::ZeroStaff:
        return 0;

    case MagicWeaponType::FlameStaff:
        return 300 + static_cast<u32>(150 * GetDifficultyMultiplier());

    case MagicWeaponType::DrainStaff:
        return 40 + static_cast<u32>(28 * GetDifficultyMultiplier());

    case MagicWeaponType::WaveStaff:
        return 350 + static_cast<u32>(200 * GetDifficultyMultiplier());
    }
}


float MagicWeapon::GetAttackRange() const
{
    switch (magicWeaponType_) {
    default:
    case MagicWeaponType::ZeroStaff:
    case MagicWeaponType::FlameStaff:
    case MagicWeaponType::DrainStaff:
        return 155.0f;

    // range is in projectile time (s)
    case MagicWeaponType::WaveStaff:
        return 2.0f;

    case MagicWeaponType::InvincibilityStaff:
        return 0.0f;
    }
}


sf::Time MagicWeapon::GetUseDelay() const
{
    switch (magicWeaponType_) {
    default:
    case MagicWeaponType::ZeroStaff:
    case MagicWeaponType::FlameStaff:
    case MagicWeaponType::DrainStaff:
        return sf::seconds(1.0f);

    case MagicWeaponType::WaveStaff:
        return sf::seconds(0.8f);

    case MagicWeaponType::InvincibilityStaff:
        return sf::seconds(30.0f);
    }
}


u32 MagicWeapon::GetManaCost() const
{
    switch (magicWeaponType_) {
    default:
    case MagicWeaponType::ZeroStaff:
        return 0;

    case MagicWeaponType::FlameStaff:
        return 200;

    case MagicWeaponType::WaveStaff:
        return 150;

    case MagicWeaponType::DrainStaff:
        return 300;

    case MagicWeaponType::InvincibilityStaff:
        return 750;
    }
}


std::string MagicWeapon::GetShortDescription() const
{
    switch (magicWeaponType_) {
    default:
        return std::string();

    case MagicWeaponType::ZeroStaff:
        return "Zero cost - zero damage";

    case MagicWeaponType::FlameStaff:
        return "Area of effect damage";

    case MagicWeaponType::DrainStaff:
        return "Area of effect health steal";

    case MagicWeaponType::InvincibilityStaff:
        return "Grants 10s of invincibility";

    case MagicWeaponType::WaveStaff:
        return "Fires a powerful projectile";
    }
}


void MagicWeapon::Use(PlayerEntity* player)
{
    if (!player || !player->GetAssignedArea() || GetAmount() <= 0) {
        return;
    }

    auto stats = player->GetStats();
    auto manaCost = GetManaCost();

    // check for enough mana
    if (stats && stats->GetMana() < manaCost) {
        Game::Get().AddMessage("You need " + std::to_string(manaCost) + " Mana to use this.");
        SetUseDelayTimeLeft(sf::seconds(0.5f));
        return;
    }
    else {
        stats->SetMana(stats->GetMana() - manaCost);
    }

    switch (magicWeaponType_) {
    default:
    case MagicWeaponType::ZeroStaff:
    case MagicWeaponType::FlameStaff:
    {
        // aoe
        auto enemiesInAoE = player->GetAssignedArea()->GetWorldEntitiesInRange<Enemy>(player->GetCenterPosition(),
            GetAttackRange());

        for (auto id : enemiesInAoE) {
            auto enemy = player->GetAssignedArea()->GetEntity<Enemy>(id.first);
            assert(enemy);

            if (enemy->GetStats() && enemy->GetStats()->IsAlive()) {
                // damage enemy
                switch (magicWeaponType_) {
                default:
                case MagicWeaponType::ZeroStaff:
                case MagicWeaponType::FlameStaff:
                    enemy->Attack(Helper::GenerateRandomInt<u32>(0, GetAttack()), DamageType::Magic);
                    break;

                case MagicWeaponType::DrainStaff:
                    player->Heal(enemy->Attack(Helper::GenerateRandomInt<u32>(0, GetAttack()), DamageType::Magic));
                    break;
                }

                enemy->MoveWithCollision(sf::Vector2f(Helper::GenerateRandomReal(-8.0f, 8.0f),
                    Helper::GenerateRandomReal(-8.0f, 8.0f)));

                // show effect
                EntityId effectId = Entity::InvalidId;

                switch (magicWeaponType_) {
                case MagicWeaponType::ZeroStaff:
                    effectId = player->GetAssignedArea()->EmplaceEntity<DamageEffectEntity>(DamageEffectType::Zero,
                        sf::seconds(0.5f));
                    GameAssets::Get().zeroBlastSound.play();
                    break;

                case MagicWeaponType::FlameStaff:
                    effectId = player->GetAssignedArea()->EmplaceEntity<DamageEffectEntity>(DamageEffectType::Flame,
                        sf::seconds(0.5f));
                    GameAssets::Get().blastSound.play();
                    break;

                case MagicWeaponType::DrainStaff:
                    effectId = player->GetAssignedArea()->EmplaceEntity<DamageEffectEntity>(DamageEffectType::Drain,
                        sf::seconds(0.5f));
                    GameAssets::Get().drainSound.play();
                    break;
                }

                if (effectId != Entity::InvalidId) {
                    player->GetAssignedArea()->GetEntity<DamageEffectEntity>(effectId)->SetCenterPosition(
                        enemy->GetCenterPosition());
                }
            }
        }

        break;
    }

    case MagicWeaponType::InvincibilityStaff:
        // invincibility
        player->SetInvincibility(sf::seconds(10.0f));
        GameAssets::Get().invincibilitySound.play();
        break;

    case MagicWeaponType::WaveStaff:
        // projectile
        sf::Vector2f projectileDir;

        switch (player->GetFacingDirection()) {
        case PlayerFacingDirection::Up:
            projectileDir = sf::Vector2f(0.0f, -1.0f);
            break;

        case PlayerFacingDirection::Down:
            projectileDir = sf::Vector2f(0.0f, 1.0f);
            break;

        case PlayerFacingDirection::Left:
            projectileDir = sf::Vector2f(-1.0f, 0.0f);
            break;

        case PlayerFacingDirection::Right:
            projectileDir = sf::Vector2f(1.0f, 0.0f);
            break;
        }

        auto projectile = player->GetAssignedArea()->GetEntity<ProjectileEntity>(
            player->GetAssignedArea()->EmplaceEntity<ProjectileEntity>(ProjectileType::PlayerMagicWave, projectileDir));
        assert(projectile);

        projectile->SetCenterPosition(player->GetCenterPosition() + projectileDir * 8.0f);
        projectile->SetDamage(Helper::GenerateRandomInt<u32>(0, GetAttack()));

        GameAssets::Get().waveSound.play();
        break;
    }

    GameAssets::Get().attackSound.play();
    player->PlayAttackAnimation(PlayerSelectedWeapon::Magic);
}


sf::Sprite MagicWeapon::GetSprite() const
{
    sf::Sprite weaponSprite(GameAssets::Get().itemsSpriteSheet);

    switch (magicWeaponType_) {
    case MagicWeaponType::ZeroStaff:
        weaponSprite.setTextureRect(sf::IntRect(16, 16, 16, 16));
        break;

    case MagicWeaponType::FlameStaff:
        weaponSprite.setTextureRect(sf::IntRect(32, 16, 16, 16));
        break;

    case MagicWeaponType::DrainStaff:
        weaponSprite.setTextureRect(sf::IntRect(48, 16, 16, 16));
        break;

    case MagicWeaponType::InvincibilityStaff:
        weaponSprite.setTextureRect(sf::IntRect(64, 16, 16, 16));
        break;

    case MagicWeaponType::WaveStaff:
        weaponSprite.setTextureRect(sf::IntRect(80, 16, 16, 16));
        break;

    default:
        return sf::Sprite();
    }

    return weaponSprite;
}


ItemEntity::ItemEntity() :
UnitEntity()
{
    SetSize(sf::Vector2f(16.0f, 16.0f));
}


ItemEntity::~ItemEntity()
{
}


void ItemEntity::Tick()
{
    if (!item_ || item_->GetAmount() <= 0) {
        MarkForDeletion();
    }
}


void ItemEntity::Render(sf::RenderTarget& target)
{
    if (item_ && item_->GetAmount() > 0) {
        auto itemSprite = item_->GetSprite();
        itemSprite.setPosition(GetPosition());

        target.draw(itemSprite);
    }
}


void ItemEntity::Use(EntityId playerId)
{
    if (item_) {
        auto playerEnt = GetAssignedArea()->GetEntity<PlayerEntity>(playerId);
        
        if (playerEnt && playerEnt->GetInventory()) {
            playerEnt->PickupItem(item_.get());
        }

        if (item_->GetAmount() <= 0) {
            MarkForDeletion();
        }
    }
}


bool ItemEntity::IsUsable(EntityId playerId) const
{
    auto area = GetAssignedArea();
    if (!area) {
        return false;
    }

    auto playerEnt = area->GetEntity<PlayerEntity>(playerId);
    if (!playerEnt || !playerEnt->GetInventory()) {
        return false;
    }

    return item_ && item_->GetAmount() > 0 && playerEnt->CanPickupItem(item_.get());
}