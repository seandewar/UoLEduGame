#include "DungeonGen.h"

#include <cassert>
#include <algorithm>

#include "Helper.h"
#include "World.h"
#include "Player.h"
#include "Stairs.h"


DungeonAreaGen::DungeonAreaGen(const GameFilesystemNode& node) :
node_(node)
{
    ConfigureGenSettings();
}


DungeonAreaGen::~DungeonAreaGen()
{
}


bool DungeonAreaGen::AddPlayerStart(WorldArea& area)
{
    // spawn the default player start
    auto defaultStartEnt = area.GetEntity<PlayerDefaultStartEntity>(area.EmplaceEntity<PlayerDefaultStartEntity>());
    if (!defaultStartEnt) {
        return false;
    }

    auto areaCenter = 0.5f * 
        sf::Vector2f(area.GetWidth() * BaseTile::TileSize.x, area.GetHeight() * BaseTile::TileSize.y);

    defaultStartEnt->SetPosition(areaCenter);

    // spawn the upstair if the fs node has a parent that the stairs can navigate to
    if (node_.GetParent()) {
        auto upstairEnt = area.GetEntity<UpStairEntity>(area.EmplaceEntity<UpStairEntity>());
        if (!upstairEnt) {
            return false;
        }

        if (!area.CheckEntRectangleWalkable(sf::FloatRect(areaCenter.x, areaCenter.y,
            upstairEnt->GetSize().x, upstairEnt->GetSize().y))) {
            fprintf(stderr, "ERR - DungeonAreaGen - No room to place UpStairEntity for player start!\n");

            area.RemoveEntity(upstairEnt->GetAssignedId());
            return false;
        }

        upstairEnt->SetCenterPosition(areaCenter);
    }

    return true;
}


bool DungeonAreaGen::CheckRoomRectanglePlaceable(WorldArea& area, u32 topX, u32 topY, u32 w, u32 h) const
{
    if (!area.IsTileLocationInBounds(topX, topY) ||
        !area.IsTileLocationInBounds(topX + w - 1, topY + h - 1)) {
        return false;
    }

    for (u32 y = topY; y < (topY + h) && y < area.GetHeight(); ++y) {
        for (u32 x = topX; x < (topX + w) && x < area.GetWidth(); ++x) {
            auto tile = area.GetTile(x, y);
            if (tile) {
                auto genericTile = dynamic_cast<const GenericTile*>(tile);
                if (!genericTile) {
                    return false;
                }

                auto genericTileType = genericTile->GetType();
                if (genericTileType != GenericTileType::PassageFloor &&
                    genericTileType != GenericTileType::PassageWall) {
                    return false;
                }
            }
        }
    }

    return true;
}


bool DungeonAreaGen::PlaceEmptyRoom(WorldArea& area, Rng& rng, u32 topX, u32 topY, u32 w, u32 h)
{
    if (w <= 2 || h <= 2) {
        assert(!"Room size too small (w or h <= 2)! Room will only be generated as walls at this size.");
        return false;
    }

    if (!CheckRoomRectanglePlaceable(area, topX, topY, w, h)) {
        return false;
    }

    GenericTileType floorType;
    switch (Helper::GenerateRandomInt(rng, 1, 5)) {
    case 1:
        floorType = GenericTileType::RoomFloor1;
        break;

    case 2:
        floorType = GenericTileType::RoomFloor2;
        break;

    case 3:
        floorType = GenericTileType::RoomFloor3;
        break;

    case 4:
        floorType = GenericTileType::RoomFloor4;
        break;

    case 5:
        floorType = GenericTileType::RoomFloor5;
        break;

    default:
        assert(!"Unknown room floor type!");
        floorType = GenericTileType::RoomFloor1;
        break;
    }

    for (u32 y = topY; y < (topY + h) && y < area.GetHeight(); ++y) {
        for (u32 x = topX; x < (topX + w) && x < area.GetWidth(); ++x) {
            if (x == topX || x == (topX + w - 1) || y == topY || y == (topY + h - 1)) {
                area.PlaceTile(x, y, GenericTileType::RoomWall);
            }
            else {
                area.SetTile(x, y, floorType);
            }
        }
    }

    return true;
}


bool DungeonAreaGen::GenerateRoom(WorldArea& area, Rng& rng, u32 topX, u32 topY, u32 w, u32 h)
{
    if (!PlaceEmptyRoom(area, rng, topX, topY, w, h)) {
        return false;
    }

    // place random amount of room exit passages
    // NOTE: this can place multiple exit passages in 1 location, which should
    // create interesting passages that hopefully don't break anything...
    int roomNumExits = Helper::GenerateRandomInt(rng, 1, std::min<int>((2 * (w + h)) / 7, 4));
    for (int i = 0; i < roomNumExits; ++i) {
        ActiveGenPassage exitPassage;

        switch (Helper::GenerateRandomInt(rng, 0, 2)) {
        case 0:
            // left side
            exitPassage.posX = topX;
            exitPassage.posY = Helper::GenerateRandomInt<Rng, u32>(rng, topY + 1, topY + h - 2);
            exitPassage.nextDirection = GenGrowDirection::Left;
            break;

        case 1:
            // right side
            exitPassage.posX = topX + w - 1;
            exitPassage.posY = Helper::GenerateRandomInt<Rng, u32>(rng, topY + 1, topY + h - 2);
            exitPassage.nextDirection = GenGrowDirection::Right;
            break;

        case 2:
            // top side
            exitPassage.posX = Helper::GenerateRandomInt<Rng, u32>(rng, topX + 1, topX + w - 2);
            exitPassage.posY = topY;
            exitPassage.nextDirection = GenGrowDirection::Up;
            break;

        case 3:
            // bottom side
            exitPassage.posX = Helper::GenerateRandomInt<Rng, u32>(rng, topX + 1, topX + w - 2);
            exitPassage.posY = topY + h - 1;
            exitPassage.nextDirection = GenGrowDirection::Down;
            break;

        default:
            assert(!"Invalid side for exit passage in room generation!");
            return false;
        }

        exitPassage.growsLeft = Helper::GenerateRandomInt(rng, roomPassageGrowAmountMin_, roomPassageGrowAmountMax_);
        newActivePassages_.emplace_back(exitPassage);
    }

    return true;
}


bool DungeonAreaGen::GenerateStructure(WorldArea& area, Rng& rng, ActiveGenPassage& passage)
{
    // don't place a structure if we've met the max OR if we met the target and
    // fail the roll, but request for the passage to be killed by returning true.
    if (currentStructureCount_ >= maxStructureAmount_ ||
        (currentStructureCount_ >= targetStructureAmount_ && !Helper::GenerateRandomBool(rng, structureChanceAfterTargetMet_))) {
        return true;
    }

    // HACKHACK this whole function is type-casting hell
    // TODO more structure types
    u32 roomW = Helper::GenerateRandomInt<Rng, u32>(rng, otherRoomsWidthMin_, otherRoomsWidthMax_);
    u32 roomH = Helper::GenerateRandomInt<Rng, u32>(rng, otherRoomsWidthMin_, otherRoomsWidthMax_);

    i64 roomX, roomY;

    switch (passage.nextDirection) {
    case GenGrowDirection::Up:
        roomX = passage.posX - Helper::GenerateRandomInt<Rng, i64>(rng, 1, roomW - 2);
        roomY = passage.posY - static_cast<i64>(roomH) + 1;
        break;

    case GenGrowDirection::Down:
        roomX = passage.posX - Helper::GenerateRandomInt<Rng, i64>(rng, 1, roomW - 2);
        roomY = passage.posY;
        break;

    case GenGrowDirection::Left:
        roomX = passage.posX - static_cast<i64>(roomW) + 1;
        roomY = passage.posY - Helper::GenerateRandomInt<Rng, i64>(rng, 1, roomH - 2);
        break;

    case GenGrowDirection::Right:
        roomX = passage.posX;
        roomY = passage.posY - Helper::GenerateRandomInt<Rng, i64>(rng, 1, roomH - 2);
        break;

    default:
        return false;
    }

    // check if x and y pos of room isnt negative - GenerateRoom()
    // should check to see if room is within max world bounds for us.
    if (roomX < 0 || roomY < 0) {
        return false;
    }

    if (GenerateRoom(area, rng, static_cast<u32>(roomX), static_cast<u32>(roomY), roomW, roomH)) {
        ++currentStructureCount_;
        return true;
    }
    else {
        return false;
    }
}


bool DungeonAreaGen::CheckPassageRectanglePlaceable(const WorldArea& area, u32 topX, u32 topY, u32 w, u32 h) const
{
    if (!area.IsTileLocationInBounds(topX, topY) ||
        !area.IsTileLocationInBounds(topX + w - 1, topY + h - 1)) {
        return false;
    }

    for (u32 y = topY; y < (topY + h) && y < area.GetHeight(); ++y) {
        for (u32 x = topX; x < (topX + w) && x < area.GetWidth(); ++x) {
            auto tile = area.GetTile(x, y);
            if (tile) {
                auto genericTile = dynamic_cast<const GenericTile*>(tile);
                if (!genericTile) {
                    return false;
                }

                auto genericTileType = genericTile->GetType();
                if (genericTileType != GenericTileType::PassageFloor &&
                    genericTileType != GenericTileType::PassageWall &&
                    genericTileType != GenericTileType::RoomWall) {
                    return false;
                }
            }
        }
    }

    return true;
}


bool DungeonAreaGen::GrowActivePassage(WorldArea& area, Rng& rng, ActiveGenPassage& passage)
{
    if (passage.growsLeft <= 0) {
        // try to place a structure here (or have a chance to do so if we have already
        // met our target room count).
        if (!GenerateStructure(area, rng, passage)) {
            // we cannot place a structure here due to lack of clearance, 
            // so allow the passage to grow some more.
            passage.growsLeft = Helper::GenerateRandomInt(rng, passageRetryGrowAmountMin_, passageRetryGrowAmountMax_);
        }
        else {
            // structure was placed here or target structure count met, kill this passage.
            return false;
        }
    }

    u32 growLength = Helper::GenerateRandomInt<Rng, u32>(rng, passageGrowLengthMin_, passageGrowLengthMax_);

    if (growLength > 0) {
        switch (passage.nextDirection) {
        case GenGrowDirection::Up:
            if (!CheckPassageRectanglePlaceable(area, passage.posX - 1, passage.posY - growLength, 3, growLength + 1) ||
                growLength + 1 > passage.posY) {
                // no room to grow, do nothing.
                break;
            }

            for (u32 i = 0; i <= growLength + 1; ++i) {
                area.PlaceTile(passage.posX - 1, passage.posY - i, GenericTileType::PassageWall);
                area.PlaceTile(passage.posX + 1, passage.posY - i, GenericTileType::PassageWall);

                if (i == growLength + 1) {
                    area.PlaceTile(passage.posX, passage.posY - i, GenericTileType::PassageWall);
                }
                else {
                    area.SetTile(passage.posX, passage.posY - i, GenericTileType::PassageFloor);
                }
            }

            passage.posY -= growLength;
            break;

        case GenGrowDirection::Down:
            if (!CheckPassageRectanglePlaceable(area, passage.posX - 1, passage.posY, 3, growLength + 1) ||
                passage.posY + growLength + 1 >= area.GetHeight()) {
                // no room to grow, do nothing.
                break;
            }

            for (u32 i = 0; i <= growLength + 1; ++i) {
                area.PlaceTile(passage.posX - 1, passage.posY + i, GenericTileType::PassageWall);
                area.PlaceTile(passage.posX + 1, passage.posY + i, GenericTileType::PassageWall);

                if (i == growLength + 1) {
                    area.PlaceTile(passage.posX, passage.posY + i, GenericTileType::PassageWall);
                }
                else {
                    area.SetTile(passage.posX, passage.posY + i, GenericTileType::PassageFloor);
                }
            }

            passage.posY += growLength;
            break;

        case GenGrowDirection::Left:
            if (!CheckPassageRectanglePlaceable(area, passage.posX - growLength, passage.posY - 1, growLength + 1, 3) ||
                growLength + 1 > passage.posX) {
                // no room to grow, do nothing.
                break;
            }

            for (u32 i = 0; i <= growLength + 1; ++i) {
                area.PlaceTile(passage.posX - i, passage.posY - 1, GenericTileType::PassageWall);
                area.PlaceTile(passage.posX - i, passage.posY + 1, GenericTileType::PassageWall);

                if (i == growLength + 1) {
                    area.PlaceTile(passage.posX - i, passage.posY, GenericTileType::PassageWall);
                }
                else {
                    area.SetTile(passage.posX - i, passage.posY, GenericTileType::PassageFloor);
                }
            }

            passage.posX -= growLength;
            break;

        case GenGrowDirection::Right:
            if (!CheckPassageRectanglePlaceable(area, passage.posX, passage.posY - 1, growLength + 1, 3) ||
                passage.posX + growLength + 1 >= area.GetWidth()) {
                // no room to grow, do nothing.
                break;
            }

            for (u32 i = 0; i <= growLength + 1; ++i) {
                area.PlaceTile(passage.posX + i, passage.posY - 1, GenericTileType::PassageWall);
                area.PlaceTile(passage.posX + i, passage.posY + 1, GenericTileType::PassageWall);

                if (i == growLength + 1) {
                    area.PlaceTile(passage.posX + i, passage.posY, GenericTileType::PassageWall);
                }
                else {
                    area.SetTile(passage.posX + i, passage.posY, GenericTileType::PassageFloor);
                }
            }

            passage.posX += growLength;
            break;

        case GenGrowDirection::None:
            // don't bother growing passages with no GenGrowDirection set
            return true;

        default:
            assert(!"Invalid GenGrowDirection for ActiveGenPassage!");
            return false;
        }
    }

    // decrement amount of passage grows left & choose another grow direction for the next growth
    --passage.growsLeft;

    if (passage.growsLeft > 0) {
        switch (Helper::GenerateRandomInt(rng, 0, 2)) {
        case 0:
            // go in the same direction again.
            break;

        case 1:
            switch (passage.nextDirection) {
            case GenGrowDirection::Up:
                passage.nextDirection = GenGrowDirection::Left;
                break;

            case GenGrowDirection::Down:
                passage.nextDirection = GenGrowDirection::Right;
                break;

            case GenGrowDirection::Left:
                passage.nextDirection = GenGrowDirection::Down;
                break;

            case GenGrowDirection::Right:
                passage.nextDirection = GenGrowDirection::Up;
                break;
            }
            break;

        case 2:
            switch (passage.nextDirection) {
            case GenGrowDirection::Up:
                passage.nextDirection = GenGrowDirection::Right;
                break;

            case GenGrowDirection::Down:
                passage.nextDirection = GenGrowDirection::Left;
                break;

            case GenGrowDirection::Left:
                passage.nextDirection = GenGrowDirection::Up;
                break;

            case GenGrowDirection::Right:
                passage.nextDirection = GenGrowDirection::Down;
                break;
            }
            break;

        default:
            assert(!"Unknown direction for ActiveGenPassage to grow in!");
            return false;
        }
    }

    return true;
}


bool DungeonAreaGen::PlaceDownStairs(WorldArea& area, Rng& rng)
{
    for (std::size_t i = 0; i < node_.GetChildrenCount(); ++i) {
        auto childNode = node_.GetChildNode(i);

        if (childNode && childNode->IsDirectory()) {
            // try to create stairs for this node
            // TODO less bruteforcey?!?!?!?
            bool placedStair = false;
            int tryCount = 0;

            while (tryCount++ < 1000) {
                auto desiredArea = sf::FloatRect(
                    Helper::GenerateRandomInt<Rng, u32>(rng, 0, area.GetWidth() - 1) * BaseTile::TileSize.x,
                    Helper::GenerateRandomInt<Rng, u32>(rng, 0, area.GetHeight() - 1) * BaseTile::TileSize.y,
                    BaseTile::TileSize.x, BaseTile::TileSize.y
                    );

                if (area.CheckEntRectangleWalkable(desiredArea) &&
                    area.GetFirstWorldEntInRectangle(desiredArea) == Entity::InvalidId) {
                    auto downstairEnt = area.GetEntity<DownStairEntity>(
                        area.EmplaceEntity<DownStairEntity>(childNode->GetName()));
                    
                    if (downstairEnt) {
                        downstairEnt->SetPosition(sf::Vector2f(desiredArea.left, desiredArea.top));
                        placedStair = true;
                        break;
                    }
                }
            }

            if (!placedStair) {
                return false;
            }
        }
    }

    return true;
}


void DungeonAreaGen::ConfigureGenSettings()
{
    printf("DungeonAreaGen - ConfigureGenSettings(); node '%s' has random identifier %u and %u children.\n",
        GameFilesystem::GetNodePathString(node_).c_str(), node_.GetRandomIdentifier(), node_.GetChildrenCount());

    Rng rng(node_.GetRandomIdentifier());

    genSeed_ = rng();
    genIterations_ = 60;
    genMaxRetries_ = 10;

    minStructureAmount_ = node_.GetChildrenCount() / 3;
    maxStructureAmount_ = std::min<int>(14, node_.GetChildrenCount());
    targetStructureAmount_ = std::max(1, std::min<int>(node_.GetChildrenCount() / 2, maxStructureAmount_));
    structureChanceAfterTargetMet_ = 0.3;

    otherRoomsWidthMin_ = firstRoomWidthMin_ = 8;
    otherRoomsWidthMax_ = firstRoomWidthMax_ = 10 + std::min<u32>((node_.GetChildrenCount() / 2), 14);

    fallbackRoomHeight_ = fallbackRoomWidth_ = std::max<u32>(16, node_.GetChildrenCount() / 2);

    roomPassageGrowAmountMin_ = 1;
    roomPassageGrowAmountMax_ = 4;

    passageRetryGrowAmountMin_ = 1;
    passageRetryGrowAmountMax_ = 2;

    passageGrowLengthMin_ = 4;
    passageGrowLengthMax_ = 6;

    printf("DungeonAreaGen - ConfigureGenSettings(); settings for dungeon area generation:\n");
    printf("\tgenSeed: %u\n", genSeed_);
    printf("\tgenIterations: %d\n", genIterations_);
    printf("\tgenMaxRetries: %d\n", genMaxRetries_);
    printf("\tfallbackRoomSize: (%u x %u)\n", fallbackRoomWidth_, fallbackRoomHeight_);
    printf("\tminStructureAmount: %d\n", minStructureAmount_);
    printf("\tmaxStructureAmount: %d\n", maxStructureAmount_);
    printf("\ttargetStructureAmount: %d\n", targetStructureAmount_);
    printf("\tstructureChanceAfterTargetMet: %f\n", structureChanceAfterTargetMet_);
    printf("\tfirstRoomWidth: (%u, %u)\n", firstRoomWidthMin_, firstRoomWidthMax_);
    printf("\totherRoomsWidth: (%u, %u)\n", otherRoomsWidthMin_, otherRoomsWidthMax_);
    printf("\troomPassageGrowAmount: (%d, %d)\n", roomPassageGrowAmountMin_, roomPassageGrowAmountMax_);
    printf("\tpassageRetryGrowAmount: (%d, %d)\n", passageRetryGrowAmountMin_, passageRetryGrowAmountMax_);
    printf("\tpassageGrowLength: (%u, %u)\n", passageGrowLengthMin_, passageGrowLengthMax_);
    printf("\n");
}


bool DungeonAreaGen::GenerateFallbackArea(WorldArea& area, Rng& rng)
{
    fprintf(stderr, "DungeonAreaGen: WARN - Generating fallback area!!!\n");
    return GenerateCenterRoom(area, rng, fallbackRoomWidth_, fallbackRoomHeight_);
}


bool DungeonAreaGen::GenerateArea(WorldArea& area, Rng& rng)
{
    for (int i = 0; i < genIterations_; ++i) {
        if (i == 0) {
            // Place the first room.
            if (!GenerateCenterRoom(area, rng,
                Helper::GenerateRandomInt<Rng, u32>(rng, firstRoomWidthMin_, firstRoomWidthMax_),
                Helper::GenerateRandomInt<Rng, u32>(rng, firstRoomWidthMin_, firstRoomWidthMax_)
                )) {
                fprintf(stderr, "DungeonAreaGen: WARN - could not generate starting room!!!\n");
                assert(!"Failed to generate starting room!!!");
                return false;
            }

            // Place start point
            if (!AddPlayerStart(area)) {
                fprintf(stderr, "DungeonAreaGen: WARN - could not add player start!\n");
                return false;
            }
        }
        else {
            // Add new passages to be added to active list
            activePassages_.insert(activePassages_.end(), newActivePassages_.begin(), newActivePassages_.end());
            newActivePassages_.clear();

            // Grow active passages
            for (auto it = activePassages_.begin(); it != activePassages_.end();) {
                auto& passage = *it;

                if (!GrowActivePassage(area, rng, passage)) {
                    // passage has finished growing, remove it from active list
                    it = activePassages_.erase(it);
                }
                else {
                    ++it;
                }
            }
        }
    }

    // check if we met the min structure requirement, fail if we didn't.
    if (currentStructureCount_ < minStructureAmount_) {
        fprintf(stderr, "DungeonAreaGen: WARN - Failed to meet minimum structure count! (%d < %d)\n",
            currentStructureCount_, minStructureAmount_);
        
        return false;
    }
    
    if (currentStructureCount_ < targetStructureAmount_) {
        fprintf(stderr, "DungeonAreaGen: WARN - Target structure count wasn't met... (%d < %d)\n",
            currentStructureCount_, targetStructureAmount_);
    }

    // place down stairs
    if (!PlaceDownStairs(area, rng)) {
        return false;
    }

    return true;
}


std::unique_ptr<WorldArea> DungeonAreaGen::GenerateNewArea(u32 w, u32 h)
{
    printf("DungeonAreaGen: Generating new area for node '%s'\n", GameFilesystem::GetNodePathString(node_).c_str());

    Rng rng(genSeed_);
	auto area = std::make_unique<WorldArea>(&node_, w, h);

    int genTryCount = 0;
    while (true) {
        ++genTryCount;

        currentStructureCount_ = 0;
        activePassages_.clear();
        newActivePassages_.clear();

        // generate fallback area if we've used all our retries
        if (genTryCount > genMaxRetries_) {
            fprintf(stderr, "DungeonAreaGen: WARN - Failed to generate area after %d retries - generating fallback area!!\n", genMaxRetries_);

            // we've used up all our retries, fallback!
            if (!GenerateFallbackArea(*area, rng)) {
                fprintf(stderr, "DungeonAreaGen: ERROR - FAILED TO GENERATE FALLBACK AREA!\n");
                assert(!"Failed to generate fallback area!!!!");
                return nullptr;
            }

            printf("DungeonAreaGen: Finished generating FALLBACK area\n");
            break;
        }
        
        // generate area
        if (GenerateArea(*area, rng)) {
            printf("DungeonAreaGen: Finished area generation: %d structure(s) [excludes start room]\n", currentStructureCount_);
            break;
        }

        area->ClearTiles();
    }

	return area;
}