#pragma once

#include <memory>
#include <vector>
#include <unordered_map>
#include <chrono>

#include <SFML/Graphics/RenderTarget.hpp>
#include <SFML/Graphics/Shape.hpp>
#include <SFML/Graphics/View.hpp>
#include <SFML/System/Time.hpp>

#include "Types.h"
#include "Tile.h"
#include "Entity.h"
#include "Collision.h"
#include "GameFilesystem.h"

/**
* Represents an area of the game world (a dungeon floor .etc)
*/
class WorldArea
{
    struct DebugRenderableInfo
    {
    private:
        bool isTransformable_;

    public:
        sf::Time timeLeft;
        std::unique_ptr<sf::Drawable> drawable;
        std::string labelString;

        DebugRenderableInfo(const sf::Time& time, std::unique_ptr<sf::Drawable>& drawable,
            const std::string& labelString) :
            timeLeft(time),
            drawable(std::move(drawable)),
            labelString(labelString)
        {
            isTransformable_ = dynamic_cast<sf::Transformable*>(this->drawable.get()) != nullptr;
        }

        DebugRenderableInfo(DebugRenderableInfo&& other) :
            timeLeft(other.timeLeft),
            drawable(std::move(other.drawable)),
            labelString(other.labelString),
            isTransformable_(other.isTransformable_)
        { }

        inline DebugRenderableInfo& operator=(DebugRenderableInfo&& other)
        {
            timeLeft = other.timeLeft;
            drawable = std::move(other.drawable);
            labelString = other.labelString;
            isTransformable_ = other.isTransformable_;
            return *this;
        }

        /**
        * Whether or not the Renderable inherits from sf::Transformable
        */
        inline bool IsTransformable() const { return isTransformable_; }
    };

	const u32 w_, h_;
	const GameFilesystemNode* relatedNode_;

	std::vector<std::unique_ptr<BaseTile>> tiles_;

    EntityId nextEntId_;
    std::unordered_map<EntityId, std::unique_ptr<Entity>> ents_;

    sf::View renderView_;

    std::vector<DebugRenderableInfo> debugRenderables_;

    inline std::size_t GetTileIndex(u32 x, u32 y) const { return (y * w_) + x; }

	inline std::unique_ptr<BaseTile>& GetTileElement(u32 x, u32 y) { return tiles_[GetTileIndex(x, y)]; }
    inline const std::unique_ptr<BaseTile>& GetTileElement(u32 x, u32 y) const { return tiles_[GetTileIndex(x, y)]; }

public:
	WorldArea(const GameFilesystemNode* relatedNode, u32 w = 200, u32 h = 200);
	~WorldArea();

    template <typename T>
    inline void AddDebugRenderable(const sf::Time& timeToDraw, std::unique_ptr<T>& drawable,
        const std::string& labelString = std::string())
    {
        if (drawable) {
            // TODO: No idea why I have to perform a move for the conversion...?
            debugRenderables_.emplace_back(timeToDraw,
                static_cast<std::unique_ptr<sf::Drawable>>(std::move(drawable)), labelString);
        }
    }

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
    bool CheckRectangleWalkable(u32 topX, u32 topY, u32 w, u32 h) const;

    /**
    * Returns true if the rectangle r is able to move with displacement d without colliding
    * with any unwalkable tiles along the way.
    * Returns false if the rectangle r collides with a tile on the way.
    *
    * outEndPos writes the position along displacement d where there were no collisions on the way.
    * If true was returned, this position will be equal to the position of r + d. If false was returned,
    * this position will be the last position where r is not intersecting any tiles (but is touching the
    * colliding tile).
    *
    * If false was returned, outCollidedTile, outCollidedTileX and outCollidedTileY is written to with the
    * corrisponding colliding tile's info.
    */
    bool TryCollisionRectMove(const sf::FloatRect& r, const sf::Vector2f& d, sf::Vector2f* outEndPos,
        BaseTile** outCollidedTile = nullptr, u32* outCollidedTileX = nullptr, u32* outCollidedTileY = nullptr);

    template <typename T>
    EntityId AddEntity(std::unique_ptr<T>& ent)
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

    template <typename T, typename... Args>
    EntityId EmplaceEntity(Args&&... args)
    {
        return AddEntity<T>(std::make_unique<T>(std::forward<Args>(args)...));
    }

    bool RemoveEntity(EntityId id);

	void Tick();
	void Render(sf::RenderTarget& target, bool renderDebug = false);

	BaseTile* GetTile(u32 x, u32 y);
    const BaseTile* GetTile(u32 x, u32 y) const;

    template <typename T = Entity>
    T* GetEntity(EntityId id)
    {
        auto it = ents_.find(id);
        if (it == ents_.end()) {
            return nullptr;
        }

        return static_cast<T*>(it->second.get());
    }

    template <typename T = Entity>
    const T* GetEntity(EntityId id) const
    {
        auto it = ents_.find(id);
        if (it == ents_.end()) {
            return nullptr;
        }

        return static_cast<const T*>(it->second.get());
    }

    bool CenterViewOnWorldEntity(EntityId entId);
    inline sf::View& GetRenderView() { return renderView_; }

	inline const GameFilesystemNode* GetRelatedNode() const { return relatedNode_; }

	inline u32 GetWidth() const { return w_; }
	inline u32 GetHeight() const { return h_; }
};

/**
* Represents the game world.
*/
class World
{
    bool debugMode_;

    GameFilesystem& areaFs_;

    std::unordered_map<std::string, std::unique_ptr<WorldArea>> areas_;
	WorldArea* currentArea_;
    std::string currentAreaFsPath_;

public:
    World(GameFilesystem& areaFs);
	~World();

	void Tick();
	void Render(sf::RenderTarget& target);

    bool NavigateToFsArea(const std::string& fsAreaPath);

    inline GameFilesystem& GetAreaFilesystem() { return areaFs_; }
    inline WorldArea* GetCurrentArea() { return currentArea_; }
    inline std::string GetCurrentAreaFsPath() const { return currentAreaFsPath_; }

    inline void SetDebugMode(bool debugMode) { debugMode_ = debugMode; }
    inline bool IsInDebugMode() const { return debugMode_; }
};

