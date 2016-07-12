#include "World.h"

#include <cassert>


WorldArea::WorldArea(const GameFilesystemNode* relatedNode, u32 w, u32 h) :
relatedNode_(relatedNode),
tiles_(w * h),
w_(w),
h_(h),
nextEntId_(0)
{
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
}


void WorldArea::Render(sf::RenderTarget& target)
{
    // TODO render on-screen ONLY!!!!!!!!!!!!!!

    // render tiles
	for (u32 y = 0; y < h_; ++y) {
		for (u32 x = 0; x < w_; ++x) {
			auto& tile = tiles_[GetTileIndex(x, y)];

			if (tile) {
				tile->Render(target, sf::Vector2f(x * TILE_WIDTH, y * TILE_HEIGHT));
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


World::World() :
currentArea_(nullptr)
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