#include "World.h"

#include <cassert>

#include <SFML/Graphics/RectangleShape.hpp>

#include "Game.h"
#include "DungeonGen.h"


WorldArea::WorldArea(const GameFilesystemNode* relatedNode, u32 w, u32 h) :
relatedNode_(relatedNode),
tiles_(w * h),
w_(w),
h_(h),
nextEntId_(0)
{
    renderView_.setCenter(sf::Vector2f(w_ * BaseTile::TileSize.x, h_ * BaseTile::TileSize.y) * 0.5f);
}


WorldArea::~WorldArea()
{
}


void WorldArea::ClearTiles()
{
	printf("Clearing all tiles from area...\n");
	for (auto& tile : tiles_) {
		tile.reset();
	}
}


BaseTile* WorldArea::SetTile(u32 x, u32 y, std::unique_ptr<BaseTile>& tile)
{
	if (!IsTileLocationInBounds(x, y)) {
		return nullptr;
	}

    auto& targetTile = tiles_[GetTileIndex(x, y)];
	if (tile) {
		targetTile = std::move(tile);
	}

	return targetTile.get();
}


BaseTile* WorldArea::PlaceTile(u32 x, u32 y, std::unique_ptr<BaseTile>& tile)
{
    if (!IsTileLocationInBounds(x, y)) {
        return nullptr;
    }

    auto& targetTile = tiles_[GetTileIndex(x, y)];
    if (targetTile) {
        // do not replace a tile that already exists here (unlike SetTile())
        return nullptr;
    }

    if (tile) {
        targetTile = std::move(tile);
    }

    return targetTile.get();
}


bool WorldArea::RemoveTile(u32 x, u32 y)
{
	if (!IsTileLocationInBounds(x, y)) {
		return false;
	}

	auto& tile = GetTileElement(x, y);
	if (tile) {
		tile.reset();
		return true;
	}

	return false;
}


void WorldArea::Tick()
{
    // tick tiles
	for (u32 y = 0; y < h_; ++y) {
		for (u32 x = 0; x < w_; ++x) {
			auto& tile = tiles_[GetTileIndex(x, y)];

			if (tile) {
				tile->Tick();
			}
		}
	}

    // tick ents
    for (auto& entEntry : ents_) {
        auto& ent = entEntry.second;

        if (!ent->IsMarkedForDeletion()) {
            ent->Tick();
        }
    }

    // tick debug shapes timer
    for (auto it = debugShapes_.begin(); it != debugShapes_.end();) {
        auto& shapeInfo = *it;

        if (shapeInfo.timeLeft > sf::Time::Zero) {
            shapeInfo.timeLeft -= Game::FrameTimeStep;

            ++it;
        }
        else {
            it = debugShapes_.erase(it);
        }
    }
}


void WorldArea::Render(sf::RenderTarget& target)
{
    // TODO render on-screen ONLY!!!!!!!!!!!!!!
    target.setView(renderView_);

    // render tiles
	for (u32 y = 0; y < h_; ++y) {
		for (u32 x = 0; x < w_; ++x) {
			auto& tile = tiles_[GetTileIndex(x, y)];

            if (tile) {
                tile->Render(target, sf::Vector2f(x * BaseTile::TileSize.x, y * BaseTile::TileSize.y));
            }
		}
	}

    // render ents
    for (auto& entEntry : ents_) {
        auto& ent = entEntry.second;

        if (!ent->IsMarkedForDeletion()) {
            ent->Render(target);
        }
    }

    // render debug shapes
    for (auto& shapeInfo : debugShapes_) {
        assert(shapeInfo.shape);
        target.draw(*shapeInfo.shape);
    }
}


BaseTile* WorldArea::GetTile(u32 x, u32 y)
{
	if (!IsTileLocationInBounds(x, y)) {
		return nullptr;
	}

	return GetTileElement(x, y).get();
}


const BaseTile* WorldArea::GetTile(u32 x, u32 y) const
{
    if (!IsTileLocationInBounds(x, y)) {
        return nullptr;
    }

    return GetTileElement(x, y).get();
}


bool WorldArea::CheckRectanglePlaceable(u32 topX, u32 topY, u32 w, u32 h) const
{
    if (!IsTileLocationInBounds(topX + w, topY + h)) {
        return false;
    }

    for (u32 y = topY; y < (topY + h) && y < h_; ++y) {
        for (u32 x = topX; x < (topX + w) && x < w_; ++x) {
            if (GetTile(x, y)) {
                return false;
            }
        }
    }

    return true;
}


bool WorldArea::CheckRectangleWalkable(u32 topX, u32 topY, u32 w, u32 h) const
{
    if (!IsTileLocationInBounds(topX, topY)) {
        return false;
    }

    for (u32 y = topY; y < (topY + h) && y < h_; ++y) {
        for (u32 x = topX; x < (topX + w) && x < w_; ++x) {
            auto tile = GetTile(x, y);

            if (tile && !tile->IsWalkable()) {
                return false;
            }
        }
    }

    return true;
}


bool WorldArea::TryCollisionRectMove(const sf::FloatRect& r, const sf::Vector2f& d, sf::Vector2f* outEndPos,
    BaseTile** outCollidedTile, u32* outCollidedTileX, u32* outCollidedTileY)
{
    // this will hold the final pos of r when we're done calcing it
    sf::Vector2f endPos = sf::Vector2f(r.left + d.x, r.top + d.y);

    // this will hold info about the tile collision if there was one
    BaseTile* collidedTile = nullptr;
    u32 collidedTileX, collidedTileY;

    // Step in X direction first
    sf::FloatRect xCollisionRegion;

    if (d.x >= 0.0f) {
        xCollisionRegion.left = r.left + r.width;
        xCollisionRegion.top = r.top;
        xCollisionRegion.width = d.x;
        xCollisionRegion.height = r.height;
    }
    else {
        xCollisionRegion.left = r.left + d.x;
        xCollisionRegion.top = r.top;
        xCollisionRegion.width = -d.x;
        xCollisionRegion.height = r.height;
    }

    // determine the region of the tilemap to consider
    u32 xTileStartX = static_cast<u32>(std::max(0.0f, xCollisionRegion.left / BaseTile::TileSize.x));
    u32 xTileStartY = static_cast<u32>(std::max(0.0f, xCollisionRegion.top / BaseTile::TileSize.y));
    u32 xTileEndX = static_cast<u32>(std::ceil((xCollisionRegion.left + xCollisionRegion.width) / BaseTile::TileSize.x));
    u32 xTileEndY = static_cast<u32>(std::ceil((xCollisionRegion.top + xCollisionRegion.height) / BaseTile::TileSize.y));

    // Check tiles within the x collision region if the region
    // is not oob of the tilemap
    if (IsTileLocationInBounds(xTileStartX, xTileStartY)) {
        u32 iMax = std::min(xTileEndX - xTileStartX, w_ - xTileStartX);
        u32 jMax = std::min(xTileEndY - xTileStartY, h_ - xTileStartY);

        for (u32 j = 0; j < jMax; ++j) {
            for (u32 i = 0; i < iMax; ++i) {
                u32 x = xTileStartX + (d.x >= 0.0f ? i : xTileEndX - xTileStartX - i - 1);
                u32 y = xTileStartY + j;

                auto tile = GetTile(x, y);

                if (tile && !tile->IsWalkable()) {
                    // calc the final pos on x depending on our displacement's dir in x
                    endPos.x = (d.x >= 0.0f ? x * BaseTile::TileSize.x - r.width : (x + 1) * BaseTile::TileSize.x);

                    // mark this tile as collided for now,
                    // y stepping may still find the final collision
                    collidedTile = tile;
                    collidedTileX = x;
                    collidedTileY = y;
                    break;
                }
            }
        }
    }

    // Step in Y direction now
    sf::FloatRect yCollisionRegion;

    if (d.y >= 0.0f) {
        yCollisionRegion.left = endPos.x;
        yCollisionRegion.top = r.top + r.height;
        yCollisionRegion.width = r.width;
        yCollisionRegion.height = d.y;
    }
    else {
        yCollisionRegion.left = endPos.x;
        yCollisionRegion.top = r.top + d.y;
        yCollisionRegion.width = r.width;
        yCollisionRegion.height = -d.y;
    }

    // determine the region of the tilemap to consider
    u32 yTileStartX = static_cast<u32>(std::max(0.0f, yCollisionRegion.left / BaseTile::TileSize.x));
    u32 yTileStartY = static_cast<u32>(std::max(0.0f, yCollisionRegion.top / BaseTile::TileSize.y));
    u32 yTileEndX = static_cast<u32>(std::ceil((yCollisionRegion.left + yCollisionRegion.width) / BaseTile::TileSize.x));
    u32 yTileEndY = static_cast<u32>(std::ceil((yCollisionRegion.top + yCollisionRegion.height) / BaseTile::TileSize.y));

    // Check tiles within the y collision region if the region
    // is not oob of the tilemap
    if (IsTileLocationInBounds(yTileStartX, yTileStartY)) {
        u32 iMax = std::min(yTileEndX - yTileStartX, w_ - yTileStartX);
        u32 jMax = std::min(yTileEndY - yTileStartY, h_ - yTileStartY);

        for (u32 j = 0; j < jMax; ++j) {
            for (u32 i = 0; i < iMax; ++i) {
                u32 x = yTileStartX + i;
                u32 y = yTileStartY + (d.y >= 0.0f ? j : yTileEndY - yTileStartY - j - 1);

                auto tile = GetTile(x, y);

                if (tile && !tile->IsWalkable()) {
                    // calc the final pos on y depending on our displacement's dir in y
                    endPos.y = (d.y >= 0.0f ? y * BaseTile::TileSize.y - r.width : (y + 1) * BaseTile::TileSize.y);

                    // mark this tile as collided, regardless of whether we collided in x
                    collidedTile = tile;
                    collidedTileX = x;
                    collidedTileY = y;
                    break;
                }
            }
        }
    }

    // debug draw collision regions
    std::unique_ptr<sf::Shape> xCollisionDbg = std::make_unique<sf::RectangleShape>(sf::Vector2f(
        (xTileEndX - xTileStartX) * BaseTile::TileSize.x, (xTileEndY - xTileStartY) * BaseTile::TileSize.y));
    xCollisionDbg->setPosition(xTileStartX * BaseTile::TileSize.x, xTileStartY * BaseTile::TileSize.y);
    xCollisionDbg->setFillColor(sf::Color::Transparent);
    xCollisionDbg->setOutlineColor(sf::Color(255, 0, 0, 150));
    xCollisionDbg->setOutlineThickness(-1.0f);

    std::unique_ptr<sf::Shape> yCollisionDbg = std::make_unique<sf::RectangleShape>(sf::Vector2f(
        (yTileEndX - yTileStartX) * BaseTile::TileSize.x, (yTileEndY - yTileStartY) * BaseTile::TileSize.y));
    yCollisionDbg->setPosition(yTileStartX * BaseTile::TileSize.x, yTileStartY * BaseTile::TileSize.y);
    yCollisionDbg->setFillColor(sf::Color::Transparent);
    yCollisionDbg->setOutlineColor(sf::Color(0, 0, 255, 150));
    yCollisionDbg->setOutlineThickness(-1.0f);

    if (collidedTile) {
        std::unique_ptr<sf::Shape> tileCollisionDbg = std::make_unique<sf::RectangleShape>(BaseTile::TileSize);
        tileCollisionDbg->setPosition(collidedTileX * BaseTile::TileSize.x, collidedTileY * BaseTile::TileSize.y);
        tileCollisionDbg->setFillColor(sf::Color(0, 255, 0, 150));

        AddDebugShape(Game::FrameTimeStep, tileCollisionDbg);
    }

    AddDebugShape(Game::FrameTimeStep, xCollisionDbg);
    AddDebugShape(Game::FrameTimeStep, yCollisionDbg);

    // out info and return
    if (outEndPos) {
        *outEndPos = endPos;
    }

    if (collidedTile) {
        if (outCollidedTile) {
            *outCollidedTile = collidedTile;
        }

        if (outCollidedTileX) {
            *outCollidedTileX = collidedTileX;
        }

        if (outCollidedTileY) {
            *outCollidedTileY = collidedTileY;
        }
    }

    return collidedTile == nullptr;
}


EntityId WorldArea::AddEntity(std::unique_ptr<Entity>& ent)
{
    if (ent->assignedArea_ != nullptr) {
        fprintf(stderr, "WARN!! Entity %s (id: %u, area ptr: %p) already assigned to another area!\n",
            ent->GetName().c_str(), ent->GetAssignedId(), static_cast<void*>(ent->GetAssignedArea()));

        assert(!"Entity already assigned to area!");
        return Entity::InvalidId;
    }

    assert(nextEntId_ != Entity::InvalidId);

    if (!ent || nextEntId_ == Entity::InvalidId) {
        return Entity::InvalidId;
    }

    printf("Adding new ent %s (ent id %u)\n", ent->GetName().c_str(), nextEntId_);
    ent->assignedArea_ = this;
    ent->assignedId_ = nextEntId_;
    ents_.emplace(nextEntId_, std::move(ent));

    return nextEntId_++;
}


bool WorldArea::RemoveEntity(EntityId id)
{
    auto it = ents_.find(id);
    if (it == ents_.end()) {
        return false;
    }

    printf("Removing ent %s (ent id %u)\n", it->second->GetName().c_str(), id);
    it->second->assignedArea_ = nullptr;
    it->second->assignedId_ = Entity::InvalidId;
    ents_.erase(it);

    return true;
}


Entity* WorldArea::GetEntity(EntityId id)
{
    auto it = ents_.find(id);
    if (it == ents_.end()) {
        return nullptr;
    }

    return it->second.get();
}


const Entity* WorldArea::GetEntity(EntityId id) const
{
    auto it = ents_.find(id);
    if (it == ents_.end()) {
        return nullptr;
    }

    return it->second.get();
}


World::World(GameFilesystem& areaFs) :
currentArea_(nullptr),
areaFs_(areaFs)
{
}


World::~World()
{
}


void World::Tick()
{
	if (currentArea_) {
		currentArea_->Tick();
	}
}


void World::Render(sf::RenderTarget& target)
{
	if (currentArea_) {
		currentArea_->Render(target);
	}
}


bool World::NavigateToFsArea(const std::string& fsAreaPath)
{
    // check if the area has already been loaded in
    auto it = areas_.find(fsAreaPath);

    if (it != areas_.end()) {
        // area is already loaded, change to it
        currentArea_ = it->second.get();
        printf("World - current area changed to EXISTING loaded area '%s'\n", fsAreaPath.c_str());
        return true;
    }
    else {
        // area is not already loaded in, search the fs for the node to
        // load it in

        auto fsNode = areaFs_.GetNodeFromPathString(fsAreaPath);
        if (!fsNode) {
            // area node not found
            fprintf(stderr, "WARN World - could not change current area to '%s' - fs error!\n", fsAreaPath.c_str());
            return false;
        }

        // area not loaded, gen it in
        DungeonAreaGen areaGen(*fsNode);
        auto area = areaGen.GenerateNewArea();

        if (!area) {
            // area gen failed epically
            fprintf(stderr, "WARN World - could not change current area to '%s' - area gen failed!\n", fsAreaPath.c_str());
            return false;
        }

        // add area to loaded list
        auto result = areas_.emplace(GameFilesystem::GetNodePathString(*fsNode), std::move(area));
        assert(result.second);
        printf("World - current area changed to NEW loaded area '%s'\n", fsAreaPath.c_str());

        currentArea_ = result.first->second.get();
        return true;
    }
}