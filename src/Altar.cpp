#include "Altar.h"

#include "Game.h"
#include "Helper.h"


AltarEntity::AltarEntity() :
UnitEntity()
{
    SetSize(sf::Vector2f(16.0f, 16.0f));
    SetupAnimations();
}


AltarEntity::~AltarEntity()
{
}


void AltarEntity::SetupAnimations()
{
    anim_.AddFrame(sf::Sprite(GameAssets::Get().altarSpriteSheet, sf::IntRect(0, 0, 16, 16)), sf::seconds(0.25f));
    anim_.AddFrame(sf::Sprite(GameAssets::Get().altarSpriteSheet, sf::IntRect(16, 0, 16, 16)), sf::seconds(0.25f));
    anim_.AddFrame(sf::Sprite(GameAssets::Get().altarSpriteSheet, sf::IntRect(32, 0, 16, 16)), sf::seconds(0.25f));
    anim_.AddFrame(sf::Sprite(GameAssets::Get().altarSpriteSheet, sf::IntRect(48, 0, 16, 16)), sf::seconds(0.25f));
    anim_.AddFrame(sf::Sprite(GameAssets::Get().altarSpriteSheet, sf::IntRect(64, 0, 16, 16)), sf::seconds(0.5f));

    animProtected_.AddFrame(sf::Sprite(GameAssets::Get().altarSpriteSheet, sf::IntRect(0, 16, 16, 16)),
        sf::seconds(0.2f));
    animProtected_.AddFrame(sf::Sprite(GameAssets::Get().altarSpriteSheet, sf::IntRect(16, 16, 16, 16)),
        sf::seconds(0.2f));
}


bool AltarEntity::IsRevealed() const
{
    switch (Game::Get().GetDirector().GetCurrentObjectiveType()) {
    case GameObjectiveType::CollectArtefact:
    case GameObjectiveType::NotStarted:
        return false;

    default:
        return true;
    }
}


void AltarEntity::Tick()
{
    auto area = GetAssignedArea();

    if (!area || !IsRevealed()) {
        return;
    }

    anim_.Tick();
    animProtected_.Tick();

    if (Game::Get().GetDirector().GetCurrentObjectiveType() == GameObjectiveType::BossFight) {
        // hurt players touching the stairs (they should be on fire)
        auto touchedPlayers = area->GetAllWorldEntsInRectangle<PlayerEntity>(GetRectangle());

        for (auto pid : touchedPlayers) {
            auto player = area->GetEntity<PlayerEntity>(pid);
            assert(player);

            player->Attack(Helper::GenerateRandomInt<u32>(0, 250), DamageType::Magic);
            player->MoveWithCollision(sf::Vector2f(Helper::GenerateRandomReal(-5.0f, 5.0f),
                Helper::GenerateRandomReal(-5.0f, 5.0f))); // push player
        }
    }
}


void AltarEntity::Render(sf::RenderTarget& target)
{
    if (IsRevealed()) {
        sf::Sprite altarSprite;
        
        if (Game::Get().GetDirector().GetCurrentObjectiveType() == GameObjectiveType::BossFight) {
            altarSprite = animProtected_.GetCurrentFrame();
        }
        else {
            altarSprite = anim_.GetCurrentFrame();
        }

        altarSprite.setPosition(GetPosition());
        target.draw(altarSprite);
    }
}


void AltarEntity::Use(EntityId playerId)
{
    if (!IsRevealed()) {
        return;
    }

    auto area = GetAssignedArea();
    if (!area) {
        return;
    }

    auto player = area->GetEntity<PlayerEntity>(playerId);
    if (!player) {
        return;
    }

    auto& director = Game::Get().GetDirector();

    if (director.GetCurrentObjectiveType() == GameObjectiveType::CollectArtefact) {
        if (director.GetNumArtefacts() < director.GetMaxArtefacts()) {
            auto artefactsNeeded = director.GetMaxArtefacts() - director.GetNumArtefacts();
            Game::Get().AddMessage("You need " + std::to_string(artefactsNeeded) + " more artefact pieces to ascend these stairs.");
        }
        else {
            Game::Get().AddMessage("You need more artefact pieces before you can ascend these stairs.");
        }
    }
    else if (director.GetCurrentObjectiveType() == GameObjectiveType::RootArtefactAltar) {
        // TODO trigger boss fight
        Game::Get().AddMessage("TODO SUMMON BOSS NOW OK");
    }
    else if (director.GetCurrentObjectiveType() == GameObjectiveType::BossFight) {
        Game::Get().AddMessage("The staircase has been blocked off by magical flames!");
        Game::Get().AddMessage("Defeat the Dungeon Guardian to extinguish them.");
    }
    else if (director.GetCurrentObjectiveType() == GameObjectiveType::Complete) {
        // TODO ascend stairs and win game!
    }
}