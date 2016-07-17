#include "Player.h"

#include <SFML/Window/Keyboard.hpp>
#include <SFML/Graphics/RectangleShape.hpp> // TODO debug

#include "World.h"
#include "Game.h"


PlayerEntity::PlayerEntity() :
AliveEntity()
{
    SetSize(BaseTile::TileSize * 0.5f); // TODO debug?
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

    //std::unique_ptr<sf::Shape> r = std::make_unique<sf::RectangleShape>(GetSize());
    //r->setPosition(GetPosition());
    //r->setFillColor(sf::Color::Magenta);
    //GetAssignedArea()->AddDebugShape(Game::FrameTimeStep, r);

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
