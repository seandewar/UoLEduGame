#include "Game.h"

#include <chrono>

#include "GameFilesystemGen.h"
#include "DungeonGen.h"


Game::Game(sf::RenderTarget& target) :
target_(target)
{
	// TODO
	GameFilesystemGen gen(static_cast<RngInt>(std::chrono::system_clock::now().time_since_epoch().count()));
	testFs_ = std::move(gen.GenerateNewFilesystem());

    DungeonAreaGen dgen(*testFs_->GetRootNode());
	testArea_ = std::move(dgen.GenerateNewArea());

	world_.SetCurrentArea(testArea_.get());
}


Game::~Game()
{
}


void Game::RunFrame()
{
	world_.Tick();

	target_.clear();

	sf::View testView;
	testView.reset(sf::FloatRect(0.0f, 0.0f, 1.5f * 1024.0f, 1.5f * 768.0f));
	testView.setCenter(sf::Vector2f(
		TILE_WIDTH * testArea_->GetWidth() * 0.5f,
		TILE_HEIGHT * testArea_->GetHeight() * 0.5f
		));
	target_.setView(testView);

	world_.Render(target_);
}