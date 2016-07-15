#include "Collision.h"


float Collision::RectangleAABBSweep(const CollisionRectInfo& r1, const CollisionRectInfo& r2, sf::Vector2f* outNormal)
{
    // find distance between rects on their near and far sides
    sf::Vector2f entryInv, exitInv;

    if (r1.velocity.x > 0.0f) {
        entryInv.x = r2.rect.left - (r1.rect.left + r1.rect.width);
        exitInv.x = (r2.rect.left + r2.rect.width) - r1.rect.left;
    }
    else {
        entryInv.x = (r2.rect.left + r2.rect.width) - r1.rect.left;
        exitInv.x = r2.rect.left - (r1.rect.left + r1.rect.width);
    }

    if (r1.velocity.y > 0.0f) {
        entryInv.y = r2.rect.top - (r1.rect.top + r1.rect.height);
        exitInv.y = (r2.rect.top + r2.rect.height) - r1.rect.top;
    }
    else {
        entryInv.y = (r2.rect.top + r2.rect.height) - r1.rect.top;
        exitInv.y = r2.rect.top - (r1.rect.top + r1.rect.height);
    }

    // find dist of collision & leaving dist for each axis
    // prevent divisions by 0 by checking for 0 in the velo components
    sf::Vector2f entry, exit;

    if (r1.velocity.x == 0.0f) {
        entry.x = -std::numeric_limits<float>::infinity();
        exit.x = std::numeric_limits<float>::infinity();
    }
    else {
        entry.x = entryInv.x / r1.velocity.x;
        exit.x = exitInv.x / r1.velocity.x;
    }

    if (r1.velocity.y == 0.0f) {
        entry.y = -std::numeric_limits<float>::infinity();
        exit.y = std::numeric_limits<float>::infinity();
    }
    else {
        entry.y = entryInv.y / r1.velocity.y;
        exit.y = exitInv.y / r1.velocity.y;
    }

    // find the earliest or latest collision dist
    float entryDistNormalised = std::max(entry.x, entry.y);
    float exitDistNormalised = std::min(exit.x, exit.y);

    // no collision
    if (entryDistNormalised > exitDistNormalised ||
        entry.x < 0.0f && entry.y < 0.0f ||
        entry.x > 1.0f || entry.y > 1.0f) {
        if (outNormal) {
            *outNormal = sf::Vector2f();
        }

        return 1.0f;
    }

    // there was a collision, calculate the normal
    // of the surface we collided with
    if (outNormal) {
        if (entry.x > entry.y) {
            if (entryInv.x < 0.0f) {
                *outNormal = sf::Vector2f(1.0f, 0.0f);
            }
            else {
                *outNormal = sf::Vector2f(-1.0f, 0.0f);
            }
        }
        else {
            if (entryInv.y < 0.0f) {
                *outNormal = sf::Vector2f(0.0f, 1.0f);
            }
            else {
                *outNormal = sf::Vector2f(0.0f, -1.0f);
            }
        }
    }

    return entryDistNormalised;
}