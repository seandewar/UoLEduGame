#pragma once

#include "Entity.h"
#include "IPlayerUsable.h"

/**
* Altar ent class.
*/
class AltarEntity : public UnitEntity, public IPlayerUsable
{
    Animation anim_;
    Animation animProtected_;

    void SetupAnimations();

public:
    AltarEntity();
    virtual ~AltarEntity();

    virtual void Tick() override;
    virtual void Render(sf::RenderTarget& target) override;

    bool IsRevealed() const;

    virtual void Use(EntityId playerId) override;
    inline virtual std::string GetUseText() const override { return "Ascend Mysterious Staircase"; }

    inline virtual bool IsUsable(EntityId playerId) const override { return IsRevealed(); }

    inline virtual std::string GetUnitName() const override { return "Altar"; }
    inline virtual std::string GetName() const override { return "AltarEntity"; }
};

