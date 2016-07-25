#pragma once

#include "Entity.h"

#include "IPlayerUsable.h"
#include "Item.h"

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
* The type of drop table the chest may have.
* These drops are rolled for and are in addition
* dropped wit any items stored in the chest's items_ vector.
*/
enum class ChestDropTableType
{
    StoredItemsOnly,
    Normal
};

/**
* Chest entity class.
*/
class ChestEntity : public UnitEntity, public IPlayerUsable
{
    ChestType chestType_;
    ChestDropTableType chestDropTable_;
    std::string chestFsNodeName_;

    std::vector<std::unique_ptr<Item>> items_;

    bool isOpened_;

public:
    ChestEntity(ChestType chestType = ChestType::RedChest,
        ChestDropTableType dropTable = ChestDropTableType::StoredItemsOnly,
        const std::string& chestFsNodeName = std::string());
    virtual ~ChestEntity();

    virtual void Render(sf::RenderTarget& target) override;

    virtual void Use(EntityId playerId) override;

    inline virtual ChestType GetChestType() const { return chestType_; }
    inline virtual void SetChestType(ChestType chestType) { chestType_ = chestType; }

    inline virtual ChestDropTableType GetChestDropTable() const { return chestDropTable_; }
    inline virtual void SetChestDropTable(ChestDropTableType dropTable) { chestDropTable_ = dropTable; }

    inline std::string GetChestFsNodeName() const { return chestFsNodeName_; }

    inline std::vector<std::unique_ptr<Item>>& GetItems() { return items_; }
    inline const std::vector<std::unique_ptr<Item>>& GetItems() const { return items_; }

    inline virtual bool IsOpened() const { return isOpened_; }
    inline virtual void SetOpened(bool open) { isOpened_ = open; }

    inline virtual bool IsUsable(EntityId playerId) const override { return !isOpened_; }
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

