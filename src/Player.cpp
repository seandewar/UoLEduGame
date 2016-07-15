#include "Player.h"

#include <SFML/Window/Keyboard.hpp>

#include <SFML/Graphics/RectangleShape.hpp> // TODO debug


PlayerEntity::PlayerEntity() :
AliveEntity()
{
}


PlayerEntity::~PlayerEntity()
{
}


void PlayerEntity::Tick()
{
    sf::Vector2f moveDir;

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::W)) {
        moveDir.y -= 1.0f;
    }
    else if (sf::Keyboard::isKeyPressed(sf::Keyboard::S)) {
        moveDir.y += 1.0f;
    }
    else if (sf::Keyboard::isKeyPressed(sf::Keyboard::A)) {
        moveDir.x -= 1.0f;
    }
    else if (sf::Keyboard::isKeyPressed(sf::Keyboard::D)) {
        moveDir.x += 1.0f;
    }

    // TODO
}


void PlayerEntity::Render(sf::RenderTarget& target)
{
    // TODO
    sf::RectangleShape r;
    r.setPosition(GetPosition());
    r.setSize(GetSize());
    r.setFillColor(sf::Color::Magenta);
    target.draw(r);
}
