#include "Game.h"

#include <SFML/Graphics/Sprite.hpp>
#include <SFML/Window/Keyboard.hpp>

#include "Helper.h"
#include "GameFilesystemGen.h"
#include "Player.h"


#define LOAD_FROM_FILE(asset, assetPath) \
    printf("Loading asset " #assetPath "..\n"); \
    if (!asset.loadFromFile(assetPath)) { \
        fprintf(stderr, "Failed to load asset " #assetPath "!\n"); \
        return false; \
    }


bool GameAssets::LoadAssets()
{
    printf("Loading game assets...\n");

    LOAD_FROM_FILE(gameFont, "assets/PressStart2P.ttf");

    LOAD_FROM_FILE(viewVignette, "assets/DarknessVignette.png");
    LOAD_FROM_FILE(genericTilesSheet, "assets/GenericTiles.png");
    return true;
}


const sf::Time Game::FrameTimeStep = sf::microseconds(16667);


Game::Game(sf::RenderTarget& target) :
debugMode_(false),
target_(target),
playerId_(Entity::InvalidId)
{
}


Game::~Game()
{
}


bool Game::Init()
{
    return NewGame();
}


bool Game::SpawnPlayer(const sf::Vector2f& pos)
{
    playerId_ = GetWorldArea()->EmplaceEntity<PlayerEntity>();
    auto player = GetWorldArea()->GetEntity<PlayerEntity>(playerId_);
    if (!player) {
        return false;
    }

    player->SetPosition(pos);
    return true;
}


void Game::ViewFollowPlayer()
{
    auto area = GetWorldArea();

    if (area && playerId_ != Entity::InvalidId) {
        area->GetRenderView().setSize(250.0f, 250.0f);
        area->CenterViewOnWorldEntity(playerId_);
    }
}


bool Game::NewGame()
{
    GameFilesystemGen gen;
    if (!(worldFs_ = std::move(gen.GenerateNewFilesystem()))) {
        return false;
    }

    world_ = std::make_unique<World>(*worldFs_);
    if (!world_->NavigateToFsArea("/")) {
        return false;
    }

    SpawnPlayer(0.5f * sf::Vector2f(
        BaseTile::TileSize.x * GetWorldArea()->GetWidth(),
        BaseTile::TileSize.y * GetWorldArea()->GetHeight()
        )); // TODO spawn near staircase!!

    return true;
}


void Game::Tick()
{
    if (world_) {
        world_->SetDebugMode(debugMode_);
        world_->Tick();

        // focus view on player (unless in debug mode & zooming out the map)
        if (debugMode_ && sf::Keyboard::isKeyPressed(sf::Keyboard::LControl)) {
            auto area = GetWorldArea();
            if (area) {
                // center on map and zoom out

                area->GetRenderView().setCenter(
                    0.5f * area->GetWidth() * BaseTile::TileSize.x, 
                    0.5f * area->GetHeight() * BaseTile::TileSize.y
                    );

                area->GetRenderView().setSize(
                    area->GetWidth() * BaseTile::TileSize.x,
                    area->GetHeight() * BaseTile::TileSize.y
                    );
            }
        }
        else {
            ViewFollowPlayer();
        }
    }
}


void Game::RenderVignette()
{
    sf::Sprite vignetteSprite(GameAssets::Get().viewVignette);
    vignetteSprite.setScale(
        target_.getView().getSize().x / vignetteSprite.getTextureRect().width,
        target_.getView().getSize().y / vignetteSprite.getTextureRect().height
        );

    target_.draw(vignetteSprite);
}


void Game::RenderUILocation()
{
    if (world_) {
        sf::Text locationText("Location: " + world_->GetCurrentAreaFsPath(), GameAssets::Get().gameFont, 28);
        locationText.setPosition(5.0f, 5.0f);
        locationText.setColor(sf::Color(255, 255, 255));

        Helper::RenderTextWithDropShadow(target_, locationText);
    }
}


void Game::Render()
{
    target_.clear();

    if (world_) {
        world_->Render(target_);

        RenderVignette();
        RenderUILocation();
    }
}