#pragma once

#include "Entity.h"
#include "IPlayerUsable.h"

/**
* Altar ent class.
*/
class AltarEntity : public UnitEntity, public IPlayerUsable
{
public:
    AltarEntity();
    virtual ~AltarEntity();

    virtual void Render(sf::RenderTarget& target) override;

    virtual void Use(EntityId playerId) override;
    inline virtual std::string GetUseText() const override { return "Use Artefact Altar"; }

    inline virtual bool IsUsable(EntityId playerId) const override { return true; }

    inline virtual std::string GetUnitName() const override { return "Altar"; }
    inline virtual std::string GetName() const override { return "AltarEntity"; }
};

