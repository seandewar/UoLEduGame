#pragma once

#include "Entity.h"

#include "Item.h"

/**
* Entity where the player will be positioned if there are no other
* spawns available (e.g. root dir floor w/o a staircase to parent floor)
*/
class PlayerDefaultStartEntity : public WorldEntity
{
public:
    PlayerDefaultStartEntity();
    virtual ~PlayerDefaultStartEntity();

    inline virtual std::string GetName() const { return "PlayerDefaultStartEntity"; }
};

/**
* Player ent class.
*/
class PlayerEntity : public AliveEntity
{
    float useRange_;
    EntityId targettedUsableEnt_;

    ItemContainer* items_;

    void HandleMovement();
    void HandleUseNearbyObjects();

public:
    PlayerEntity();
    virtual ~PlayerEntity();

    virtual void Tick() override;
    virtual void Render(sf::RenderTarget& target) override;

    inline void SetItems(ItemContainer* items) { items_ = items; }

    inline ItemContainer* GetItems() { return items_; }
    inline const ItemContainer* GetItems() const { return items_; }

    virtual bool SetPositionToDefaultStart();

    inline void SetUseRange(float useRange) { useRange_ = useRange; }
    inline float GetUseRange() const { return useRange_; }

    inline EntityId GetTargettedUsableEntity() const { return targettedUsableEnt_; }

    inline virtual std::string GetUnitName() const override { return "Player"; }

    inline virtual std::string GetName() const override { return "PlayerEntity"; }
};

