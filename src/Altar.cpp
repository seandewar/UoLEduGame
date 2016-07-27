#include "Altar.h"

#include "Game.h"


AltarEntity::AltarEntity() :
UnitEntity()
{
    SetSize(sf::Vector2f(32.0f, 32.0f));
}


AltarEntity::~AltarEntity()
{
}


void AltarEntity::Render(sf::RenderTarget& target)
{
    sf::Sprite altarSprite(GameAssets::Get().altarSpriteSheet, sf::IntRect(0, 0, 32, 32));
    altarSprite.setPosition(GetPosition());

    target.draw(altarSprite);
}


void AltarEntity::Use(EntityId playerId)
{
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
            Game::Get().AddMessage("You need " + std::to_string(artefactsNeeded) + " more artefact pieces to use the altar.");
        }
        else {
            Game::Get().AddMessage("You need more artefact pieces before you can use the altar.");
        }
    }
    else if (director.GetCurrentObjectiveType() == GameObjectiveType::RootArtefactAltar) {
        // TODO trigger boss fight
        Game::Get().AddMessage("TODO SUMMON BOSS NOW OK");
    }
    else if (director.GetCurrentObjectiveType() == GameObjectiveType::BossFight) {
        Game::Get().AddMessage("You need to defeat the altar guardian first!");
    }
}