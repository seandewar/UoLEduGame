#pragma once

#include <SFML/Graphics/Rect.hpp>

/**
* Struct containing info about a collidable rectangle.
*/
struct CollisionRectInfo
{
    sf::FloatRect rect;
    sf::Vector2f velocity;

    CollisionRectInfo() { }

    CollisionRectInfo(const sf::FloatRect& rect, const sf::Vector2f& velocity = sf::Vector2f()) :
        rect(rect),
        velocity(velocity)
    { }
};

/**
* Class containing collision handling functions
*/
class Collision
{
public:
    /**
    * Gets the approximate region where an AABB sweep collision may occur for the given
    * rectangle. Only rectangles within the rect area returned should be checked for
    * an AABB sweeping collision with the rectangle r provided as an essential optimisation.
    *
    * Credit to: http://www.gamedev.net/page/resources/_/technical/game-programming/swept-aabb-collision-detection-and-response-r3084
    */
    static sf::FloatRect GetAABBSweepBroadphaseRegion(const CollisionRectInfo& r);

    /**
    * Does an AABB sweep using the two rectangles r1 and r2.
    * A time between 0.0f and 1.0f is returned.
    * indicating when the collision took place.
    *
    * A value of 0.0f indicates a collision at the start of the sweep.
    * A value of 1.0f can be assumed there was no sweeping collision.
    *
    * Credit to: http://www.gamedev.net/page/resources/_/technical/game-programming/swept-aabb-collision-detection-and-response-r3084
    */
    static float RectangleAABBSweep(const CollisionRectInfo& r1, const CollisionRectInfo& r2, sf::Vector2f* outNormal);
};

