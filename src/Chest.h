#pragma once

#include "Entity.h"

#include "IPlayerUsable.h"

/**
* The type of appearance for ChestEntity.
*/
enum class ChestType
{
    RedChest,
    BlueChest,
    PurpleChest
};

/**
* Chest entity class.
*/
class ChestEntity : public UnitEntity, public IPlayerUsable
{
    ChestType chestType_;
    std::string chestFsNodeName_;

    bool isOpened_;

public:
    ChestEntity(ChestType chestType = ChestType::RedChest, const std::string& chestFsNodeName = std::string());
    virtual ~ChestEntity();

    virtual void Render(sf::RenderTarget& target) override;

    virtual void Use(EntityId playerId) override;

    inline virtual ChestType GetChestType() const { return chestType_; }
    inline virtual void SetChestType(ChestType chestType) { chestType_ = chestType; }

    inline std::string GetChestFsNodeName() const { return chestFsNodeName_; }

    inline virtual bool IsOpened() const { return isOpened_; }
    inline virtual void SetOpened(bool open) { isOpened_ = open; }

    inline virtual bool IsUsable() const override { return !isOpened_; }
    inline virtual std::string GetUseText() const
    { 
        if (chestFsNodeName_.empty()) {
            return "Open chest";
        }
        else {
            return std::string("Open the ") + chestFsNodeName_ + " chest";
        }
    }

    inline virtual std::string GetUnitName() const override { return "Chest"; }

    inline virtual std::string GetName() const override { return "ChestEntity"; }
};

