#pragma once

#include <memory>
#include <vector>
#include <unordered_map>

#include <SFML/Graphics/RenderTarget.hpp>

#include "Types.h"
#include "Tile.h"
#include "Entity.h"

class GameFilesystemNode;

/**
* Represents an area of the game world (a dungeon floor .etc)
*/
class WorldArea
{
	const u32 w_, h_;
	const GameFilesystemNode* relatedNode_;
	std::vector<std::unique_ptr<BaseTile>> tiles_;

    EntityId nextEntId_;
    std::unordered_map<EntityId, std::unique_ptr<Entity>> ents_;

    inline std::size_t GetTileIndex(u32 x, u32 y) const { return (y * w_) + x; }

	inline std::unique_ptr<BaseTile>& GetTileElement(u32 x, u32 y) { return tiles_[GetTileIndex(x, y)]; }
    inline const std::unique_ptr<BaseTile>& GetTileElement(u32 x, u32 y) const { return tiles_[GetTileIndex(x, y)]; }

public:
	WorldArea(const GameFilesystemNode* relatedNode, u32 w = 200, u32 h = 200);
	~WorldArea();

    inline bool IsTileLocationInBounds(u32 x, u32 y) const { return x < w_ && y < h_; }

	void ClearTiles();

	BaseTile* SetTile(u32 x, u32 y, std::unique_ptr<BaseTile>& tile);
	inline BaseTile* SetTile(u32 x, u32 y, GenericTileType type)
	{
        std::unique_ptr<BaseTile> tile = std::make_unique<GenericTile>(type);
		return SetTile(x, y, tile);
	}

    /**
    * PlaceTile() acts like SetTile(), but fails if a tile is already at the position.
    */
    BaseTile* PlaceTile(u32 x, u32 y, std::unique_ptr<BaseTile>& tile);
    inline BaseTile* PlaceTile(u32 x, u32 y, GenericTileType type)
    {
        std::unique_ptr<BaseTile> tile = std::make_unique<GenericTile>(type);
        return PlaceTile(x, y, tile);
    }

	bool RemoveTile(u32 x, u32 y);

	template <typename TileT>
	void FillCircle(u32 centerX, u32 centerY, u32 r, const TileT* tile)
	{
		u32 startX = ((centerX - static_cast<i64>(r)) < 0 ? 0 : centerX - r);
		u32 startY = ((centerY - static_cast<i64>(r)) < 0 ? 0 : centerY - r);

		for (u32 y = startY; y <= (centerY + r) && y < h_; ++y) {
			for (u32 x = startX; x <= (centerX + r) && x < w_; ++x) {
				if (((x - centerX) * (x - centerX)) + ((y - centerY) * (y - centerY)) <= r * r) {
					if (tile) {
						SetTile(x, y, static_cast<std::unique_ptr<BaseTile>>(std::make_unique<TileT>(*tile)));
					}
					else {
						RemoveTile(x, y);
					}
				}
			}
		}
	}

	inline void FillCircle(u32 centerX, u32 centerY, u32 r, GenericTileType type)
	{
		FillCircle<GenericTile>(centerX, centerY, r, &GenericTile(type));
	}

	template <typename TileT>
	void FillRectangle(u32 startX, u32 startY, u32 w, u32 h, const TileT* tile)
	{
		for (u32 y = startY; y <= (startY + h) && y < h_; ++y) {
			for (u32 x = startX; x <= (startX + w) && x < w_; ++x) {
				if (tile) {
					SetTile(x, y, static_cast<std::unique_ptr<BaseTile>>(std::make_unique<TileT>(*tile)));
				}
				else {
					RemoveTile(x, y);
				}
			}
		}
	}

	inline void FillRectangle(u32 startX, u32 startY, u32 w, u32 h, GenericTileType type)
	{
		FillRectangle<GenericTile>(startX, startY, w, h, &GenericTile(type));
	}

    bool CheckRectanglePlaceable(u32 topX, u32 topY, u32 w, u32 h) const;

    EntityId AddEntity(std::unique_ptr<Entity>& ent);
    bool RemoveEntity(EntityId id);

	void Tick();
	void Render(sf::RenderTarget& target);

	BaseTile* GetTile(u32 x, u32 y);
    const BaseTile* GetTile(u32 x, u32 y) const;

    Entity* GetEntity(EntityId id);
    const Entity* GetEntity(EntityId id) const;

	inline const GameFilesystemNode* GetRelatedNode() const { return relatedNode_; }

	inline u32 GetWidth() const { return w_; }
	inline u32 GetHeight() const { return h_; }
};

/**
* Represents the game world.
*/
class World
{
	WorldArea* currentArea_;

public:
	World();
	~World();

	void Tick();
	void Render(sf::RenderTarget& target);

	inline void SetCurrentArea(WorldArea* area) { currentArea_ = area; }
	inline WorldArea* GetCurrentArea() { return currentArea_; }
};

