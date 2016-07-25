#include "Altar.h"

#include "Game.h"


AltarEntity::AltarEntity() :
UnitEntity()
{
}


AltarEntity::~AltarEntity()
{
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
            Game::Get().AddMessage("You need " + 
                std::to_string()
        }
        else {

        }

        return;
    }
}