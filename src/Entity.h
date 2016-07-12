#pragma once

#include <string>

#include <SFML/Graphics/RenderTarget.hpp>

#include "Types.h"

typedef u64 EntityId;

class WorldArea;

/**
* Base entity class.
*/
class Entity
{
    // WorldArea needs to be able to set assignedId_ and assignedArea_
    friend class WorldArea;

    EntityId assignedId_;
    WorldArea* assignedArea_;
    bool markedForDeletion_;

public:
    static const EntityId InvalidId = UINT64_MAX;

    Entity();
    virtual ~Entity();

    inline virtual void Tick() { }
    inline virtual void Render(sf::RenderTarget& target) { }

    inline void MarkForDeletion() { markedForDeletion_ = true; }
    inline bool IsMarkedForDeletion() const { return markedForDeletion_; }

    inline EntityId GetAssignedId() const { return assignedId_; }
    inline WorldArea* GetAssignedArea() const { return assignedArea_; }

    virtual std::string GetName() const = 0;
};

/**
* Base class for an ent inside of the world with a pos and size.
*/
class WorldEntity : public Entity
{
    sf::FloatRect rect_;

public:
    WorldEntity();
    virtual ~WorldEntity();

    inline void SetRectangle(const sf::FloatRect& rect) { rect_ = rect; }
    inline sf::FloatRect GetRectangle() const { return rect_; }

    inline void SetPosition(const sf::Vector2f& pos) { rect_.left = pos.x; rect_.top = pos.y; }
    inline sf::Vector2f GetPosition() const { return sf::Vector2f(rect_.left, rect_.top); }

    inline void SetSize(const sf::Vector2f& size) { rect_.width = size.x; rect_.height = size.y; }
    inline sf::Vector2f GetSize() const { return sf::Vector2f(rect_.width, rect_.height); }
};
