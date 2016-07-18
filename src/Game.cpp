#include "Game.h"

#include "GameFilesystemGen.h"


const sf::Time Game::FrameTimeStep = sf::microseconds(16667);


Game::Game(sf::RenderTarget& target) :
target_(target)
{
	GameFilesystemGen gen;
    worldFs_ = std::move(gen.GenerateNewFilesystem());
    world_ = std::make_unique<World>(*worldFs_);
    assert(world_->NavigateToFsArea("/"));
}


Game::~Game()
{
}


void Game::RunFrame()
{
    Tick();
    Render();
}


void Game::Tick()
{
    if (world_) {
        world_->Tick();
    }
}


void Game::Render()
{
    target_.clear();

    if (world_) {
        world_->Render(target_);
    }
}