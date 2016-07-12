#pragma once

#include <memory>

#include <SFML/Graphics/RenderTarget.hpp>

#include "GameFilesystem.h"
#include "World.h"

/**
* Main Game class
*/
class Game
{
	sf::RenderTarget& target_;

	std::unique_ptr<GameFilesystem> testFs_;
	std::unique_ptr<WorldArea> testArea_;

	World world_;

public:
	Game(sf::RenderTarget& target);
	~Game();

	void RunFrame();
};

