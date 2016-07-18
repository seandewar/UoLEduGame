#pragma once

#include <memory>

#include <SFML/Graphics/RenderTarget.hpp>
#include <SFML/System/Time.hpp>

#include "GameFilesystem.h"
#include "World.h"

/**
* Main Game class
*/
class Game
{
	sf::RenderTarget& target_;

    std::unique_ptr<GameFilesystem> worldFs_;
	std::unique_ptr<World> world_;

public:
    static const sf::Time FrameTimeStep;

	Game(sf::RenderTarget& target);
	~Game();

	void RunFrame();

    void Tick();
    void Render();
};

