#pragma once

#include "Entity.h"
#include "IPlayerUsable.h"

/**
* The type of StairEntity
*/
enum class StairEntityType
{
    Up,
    Down
};

/**
* Base StairEntity class for stairs
*/
class StairEntity : public UnitEntity, public IPlayerUsable
{
public:
    StairEntity();
    virtual ~StairEntity();

    inline virtual bool IsUsable(EntityId playerId) const override { return true; }

    virtual StairEntityType GetStairType() const = 0;
};

/**
* Entity for stairs to navigate to parent floor.
*/
class UpStairEntity : public StairEntity
{
public:
    UpStairEntity();
    virtual ~UpStairEntity();

    virtual void Render(sf::RenderTarget& target) override;

    virtual bool IsUsable(EntityId playerId) const override;
    virtual void Use(EntityId playerId) override;

    inline virtual std::string GetUseText() const override { return "Ascend to .."; }

    inline virtual StairEntityType GetStairType() const override { return StairEntityType::Up; }
    inline virtual std::string GetUnitName() const override { return "Staircase to .."; }

    inline virtual std::string GetName() const override { return "UpStairEntity"; }
};

/**
* Entity for stairs to navigate to different child dungeon floors.
*/
class DownStairEntity : public StairEntity
{
    std::string destinationFsNodeName_;

public:
    DownStairEntity(const std::string& destinationFsNodeName = std::string());
    virtual ~DownStairEntity();

    virtual void Render(sf::RenderTarget& target) override;

    inline std::string GetDestinationFsNodeName() const { return destinationFsNodeName_; }

    virtual bool IsUsable(EntityId playerId) const override;
    inline virtual void Use(EntityId playerId) override;

    inline virtual std::string GetUseText() const override { return "Descend to " + destinationFsNodeName_; }

    inline virtual StairEntityType GetStairType() const override { return StairEntityType::Down; }
    inline virtual std::string GetUnitName() const override { return "Staircase to " + destinationFsNodeName_; }

    inline virtual std::string GetName() const override { return "DownStairEntity"; }
};
