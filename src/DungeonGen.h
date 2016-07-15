#pragma once

#include "Types.h"
#include "GameFilesystem.h"
#include "World.h"

/**
* Generates the dungeon from the virtual filesystem's structure.
*/
class DungeonAreaGen
{
    enum class GenGrowDirection {
        None,
        Up,
        Down,
        Left,
        Right
    };

    struct ActiveGenPassage {
        u32 posX, posY;
        int growsLeft;
        GenGrowDirection nextDirection;

        ActiveGenPassage() :
            posX(0), posY(0), growsLeft(0), nextDirection(GenGrowDirection::None)
        { }

        ActiveGenPassage(u32 x, u32 y, int growAmount, GenGrowDirection nextDir = GenGrowDirection::None) :
            posX(x), posY(y), growsLeft(growAmount), nextDirection(nextDir)
        { }

        inline bool operator==(const ActiveGenPassage& other) const
        {
            // considered equivalent if both ActiveGenPassages have the same positions
            return posX == other.posX && posY == other.posY;
        }

        inline bool operator!=(const ActiveGenPassage& other) const
        {
            return !(*this == other);
        }
    };

    const GameFilesystemNode& node_;

    RngInt genSeed_;
    int genIterations_;
    int minStructureAmount_, maxStructureAmount_, targetStructureAmount_;
    int genMaxRetries_;
    double structureChanceAfterTargetMet_;
    u32 firstRoomWidthMin_, firstRoomWidthMax_;
    u32 otherRoomsWidthMin_, otherRoomsWidthMax_;
    u32 fallbackRoomWidth_, fallbackRoomHeight_;
    int roomPassageGrowAmountMin_, roomPassageGrowAmountMax_;
    int passageRetryGrowAmountMin_, passageRetryGrowAmountMax_;
    u32 passageGrowLengthMin_, passageGrowLengthMax_;

    std::vector<ActiveGenPassage> newActivePassages_;
    std::vector<ActiveGenPassage> activePassages_;

    int currentStructureCount_;

    bool CheckRoomRectanglePlaceable(WorldArea& area, u32 topX, u32 topY, u32 w, u32 h) const;
	bool PlaceEmptyRoom(WorldArea& area, u32 topX, u32 topY, u32 w, u32 h);
    bool GenerateRoom(WorldArea& area, Rng& rng, u32 topX, u32 topY, u32 w, u32 h);

    inline bool GenerateCenterRoom(WorldArea& area, Rng& rng, u32 w, u32 h)
    {
        return GenerateRoom(area, rng, (area.GetWidth() - w) / 2, (area.GetHeight() - h) / 2, w, h);
    }

    bool GenerateStructure(WorldArea& area, Rng& rng, ActiveGenPassage& entrancePassage);

    bool CheckPassageRectanglePlaceable(const WorldArea& area, u32 topX, u32 topY, u32 w, u32 h) const;
    bool GrowActivePassage(WorldArea& area, Rng& rng, ActiveGenPassage& passage);

    bool GenerateFallbackArea(WorldArea& area, Rng& rng);
    bool GenerateArea(WorldArea& area, Rng& rng);

public:
    DungeonAreaGen(const GameFilesystemNode& node);
	~DungeonAreaGen();

    void ConfigureGenSettings();
    std::unique_ptr<WorldArea> GenerateNewArea(u32 w = 200, u32 h = 200);
};

