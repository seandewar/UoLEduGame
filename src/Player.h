#pragma once

#include "Entity.h"

/**
* Player ent class.
*/
class PlayerEntity : public AliveEntity
{
public:
    PlayerEntity();
    virtual ~PlayerEntity();

    virtual void Tick() override;
    virtual void Render(sf::RenderTarget& target) override;

    inline virtual std::string GetUnitName() const override { return "Player"; }

    inline virtual std::string GetName() const override { return "PlayerEntity"; }
};

