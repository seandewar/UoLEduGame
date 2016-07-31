#include "Chest.h"

#include <SFML/Graphics/Sprite.hpp>

#include "Game.h"
#include "Tile.h"
#include "Helper.h"


ChestEntity::ChestEntity(ChestType chestType, ChestDropTableType dropTable,
    const std::string& chestFsNodeName) :
UnitEntity(),
chestType_(chestType),
chestDropTable_(dropTable),
isOpened_(false),
chestFsNodeName_(chestFsNodeName)
{
    SetSize(BaseTile::TileSize);
}


ChestEntity::~ChestEntity()
{
}


void ChestEntity::Use(EntityId playerId)
{
    if (!isOpened_) {
        // check if chest is the artefact chest
        auto parentNode = GetAssignedArea()->GetRelatedNode();

        if (Game::Get().GetDirector().GetCurrentObjectiveType() == GameObjectiveType::CollectArtefact &&
            parentNode && !chestFsNodeName_.empty()) {
            auto artefactNode = Game::Get().GetDirector().GetCurrentArtefactNode();

            if (artefactNode && parentNode->GetChildNode(chestFsNodeName_) == artefactNode) {
                switch (Game::Get().GetDirector().GetQuestionAnswerResult()) {
                case GameQuestionAnswerResult::Unanswered:
                case GameQuestionAnswerResult::Wrong:
                    Game::Get().SetDisplayedQuestion(Game::Get().GetDirector().GetCurrentQuestion());
                    return;

                case GameQuestionAnswerResult::Correct:
                    // add artefact to drops!
                    ArtefactType artefactType;

                    switch (Helper::GenerateRandomInt(1, 5)) {
                    default:
                    case 1:
                        artefactType = ArtefactType::Appearance1;
                        break;

                    case 2:
                        artefactType = ArtefactType::Appearance2;
                        break;

                    case 3:
                        artefactType = ArtefactType::Appearance3;
                        break;

                    case 4:
                        artefactType = ArtefactType::Appearance4;
                        break;

                    case 5:
                        artefactType = ArtefactType::Appearance5;
                        break;
                    }

                    items_.emplace_back(std::make_unique<ArtefactItem>(artefactType));
                    break;
                }
            }
        }

        isOpened_ = true;

        // roll drop tables
        switch (chestDropTable_) {
        case ChestDropTableType::StoredItemsOnly:
        default:
            break;

        case ChestDropTableType::Normal:
            // roll for health pot
            if (Helper::GenerateRandomBool(1 / 8.0f)) {
                items_.emplace_back(std::make_unique<PotionItem>(ItemType::HealthPotion, 1));
            }

            // roll for magic pot
            if (Helper::GenerateRandomBool(1 / 8.0f)) {
                items_.emplace_back(std::make_unique<PotionItem>(ItemType::MagicPotion, 1));
            }

            // roll for melee weapon
            if (Helper::GenerateRandomBool(1 / 12.0f)) {
                std::unique_ptr<MeleeWeapon> weapon;
                
                auto randNum = Helper::GenerateRandomInt(1, 100);

                if (randNum <= 35) {
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
                items_.emplace_back(std::move(weapon));
            }

            // roll for magic weapon
            if (Helper::GenerateRandomBool(1 / 22.0f)) {
                std::unique_ptr<MagicWeapon> weapon;

                auto randNum = Helper::GenerateRandomInt(1, 100);

                if (randNum <= 10) {
                    weapon = std::make_unique<MagicWeapon>(MagicWeaponType::InvincibilityStaff);
                }
                else if (randNum <= 40) {
                    weapon = std::make_unique<MagicWeapon>(MagicWeaponType::DrainStaff);
                }
                else {
                    weapon = std::make_unique<MagicWeapon>(MagicWeaponType::FlameStaff);
                }

                weapon->SetDifficultyMultiplier(Game::Get().GetDirector().GetCurrentDifficultyMultiplier());
                items_.emplace_back(std::move(weapon));
            }

            // roll for armour
            if (Helper::GenerateRandomBool(1 / 22.0f)) {
                std::unique_ptr<Armour> armour;

                auto randNum = Helper::GenerateRandomInt(1, 100);

                if (randNum <= 60) {
                    armour = std::make_unique<Armour>(ArmourType::WarriorHelmet);
                }
                else if (randNum <= 85) {
                    armour = std::make_unique<Armour>(ArmourType::AntiMagicVisor);
                }
                else {
                    armour = std::make_unique<Armour>(ArmourType::BalanceHeadgear);
                }

                armour->SetDifficultyMultiplier(Game::Get().GetDirector().GetCurrentDifficultyMultiplier());
                items_.emplace_back(std::move(armour));
            }
            break;
        }

        // drop items in world using ItemEntity
        auto area = GetAssignedArea();

        for (auto& item : items_) {
            auto itemEnt = area->GetEntity<ItemEntity>(area->EmplaceEntity<ItemEntity>());
            itemEnt->SetItem(std::move(item));
            itemEnt->SetCenterPosition(GetCenterPosition() +
                sf::Vector2f(Helper::GenerateRandomReal(-8.0f, 8.0f), 0.0f));
        }

        if (items_.size() > 0) {
            Game::Get().AddMessage("You open the " + chestFsNodeName_ + " chest.", sf::Color(255, 165, 0));
        }
        else {
            Game::Get().AddMessage("You open the " + chestFsNodeName_ + " chest... and find nothing!",
                sf::Color(255, 165, 0));
        }

        items_.clear();
    }
}


void ChestEntity::Render(sf::RenderTarget& target)
{
    sf::Sprite chestSprite(GameAssets::Get().chestsSpriteSheet);
    chestSprite.setPosition(GetPosition());

    if (!isOpened_) {
        // closed chest sprites
        switch (chestType_) {
        default:
        case ChestType::RedChest:
            chestSprite.setTextureRect(sf::IntRect(0, 0, 16, 16));
            break;

        case ChestType::BlueChest:
            chestSprite.setTextureRect(sf::IntRect(0, 16, 16, 16));
            break;

        case ChestType::PurpleChest:
            chestSprite.setTextureRect(sf::IntRect(0, 32, 16, 16));
            break;
        }
    }
    else {
        // open chest sprites
        chestSprite.setColor(sf::Color(255, 255, 255, 100));

        switch (chestType_) {
        default:
        case ChestType::RedChest:
            chestSprite.setTextureRect(sf::IntRect(16, 0, 16, 16));
            break;

        case ChestType::BlueChest:
            chestSprite.setTextureRect(sf::IntRect(16, 16, 16, 16));
            break;

        case ChestType::PurpleChest:
            chestSprite.setTextureRect(sf::IntRect(16, 32, 16, 16));
            break;
        }
    }

    target.draw(chestSprite);

    if (!chestFsNodeName_.empty()) {
        auto area = GetAssignedArea();

        if (area) {
            auto chestText = std::make_unique<sf::Text>(chestFsNodeName_, GameAssets::Get().gameFont, 18);
            chestText->setScale(0.15f, 0.15f);
            chestText->setColor(isOpened_ ? sf::Color(150, 150, 150, 150) : sf::Color(255, 165, 0, 255));

            auto textPos = sf::Vector2f(GetCenterPosition().x, GetPosition().y) -
                sf::Vector2f(0.5f * chestText->getGlobalBounds().width, 3.0f);

            chestText->setPosition(textPos);

            area->AddFrameUIRenderable(Helper::GetTextDropShadow(*chestText, sf::Vector2f(0.5f, 0.5f),
                isOpened_ ? sf::Color(0, 0, 0, 150) : sf::Color(0, 0, 0, 255)));
            area->AddFrameUIRenderable(chestText);
        }
    }
}