#include "Game.h"

#include <chrono>

#include "GameFilesystemGen.h"
#include "DungeonGen.h"
#include "Player.h" // TODO test


const sf::Time Game::FrameTimeStep = sf::microseconds(16667);


Game::Game(sf::RenderTarget& target) :
target_(target)
{
	// TODO
	GameFilesystemGen gen(static_cast<RngInt>(std::chrono::system_clock::now().time_since_epoch().count()));
	testFs_ = std::move(gen.GenerateNewFilesystem());

    DungeonAreaGen dgen(*testFs_->GetRootNode());
	testArea_ = std::move(dgen.GenerateNewArea());

	world_.SetCurrentArea(testArea_.get());

    auto pid = testArea_->AddEntity(static_cast<std::unique_ptr<Entity>>(std::make_unique<PlayerEntity>()));
    static_cast<PlayerEntity*>(testArea_->GetEntity(pid))->SetPosition(sf::Vector2f(
        BaseTile::TileSize.x * testArea_->GetWidth() * 0.5f,
        BaseTile::TileSize.y * testArea_->GetHeight() * 0.5f
        ));
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
		BaseTile::TileSize.x * testArea_->GetWidth() * 0.5f,
		BaseTile::TileSize.y * testArea_->GetHeight() * 0.5f
		));
	target_.setView(testView);

	world_.Render(target_);
}