#include "Player.h"

#include <SFML/Window/Keyboard.hpp>
#include <SFML/Graphics/RectangleShape.hpp> // TODO debug

#include "World.h"
#include "Game.h"


PlayerEntity::PlayerEntity() :
AliveEntity()
{
    SetSize(sf::Vector2f(12.0f, 12.0f)); // TODO debug?
}


PlayerEntity::~PlayerEntity()
{
}


void PlayerEntity::Tick()
{
    sf::Vector2f moveDir;

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::W) || sf::Keyboard::isKeyPressed(sf::Keyboard::Up)) {
        moveDir.y -= 1.0f;
    }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::S) || sf::Keyboard::isKeyPressed(sf::Keyboard::Down)) {
        moveDir.y += 1.0f;
    }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::A) || sf::Keyboard::isKeyPressed(sf::Keyboard::Left)) {
        moveDir.x -= 1.0f;
    }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::D) || sf::Keyboard::isKeyPressed(sf::Keyboard::Right)) {
        moveDir.x += 1.0f;
    }

    auto d = moveDir * GetStats().GetMoveSpeed() * Game::FrameTimeStep.asSeconds();
    MoveWithCollision(d);
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
