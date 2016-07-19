#include "Game.h"

#include <SFML/Graphics/Sprite.hpp>

#include "Helper.h"
#include "GameFilesystemGen.h"
#include "Player.h"
#include "IPlayerUsable.h"

#include "Stairs.h" // TODO DEBUG


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
    LOAD_FROM_FILE(stairsSpriteSheet, "assets/StairSprites.png");
    LOAD_FROM_FILE(playerSpriteSheet, "assets/PlayerSprites.png");
    return true;
}


const sf::Time Game::FrameTimeStep = sf::microseconds(16667);


Game::Game() :
debugMode_(false),
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
    if (playerId_ == Entity::InvalidId) {
        return false;
    }

    auto player = GetPlayerEntity();

    player->SetStats(cachedPlayerStats_);
    player->SetPosition(pos);
    return true;
}


bool Game::RemovePlayer()
{
    if (!GetWorldArea() || !GetPlayerEntity() ||
        !GetWorldArea()->RemoveEntity(playerId_)) {
        playerId_ = Entity::InvalidId;
        return false;
    }

    playerId_ = Entity::InvalidId;
    return true;
}


bool Game::ChangeLevel(const std::string& fsNodePath)
{
    if (!world_ || !world_->PreloadFsArea(fsNodePath)) {
        return false;
    }

    auto player = GetPlayerEntity();
    if (GetWorldArea() && player) {
        cachedPlayerStats_ = player->GetStats();
        RemovePlayer();
    }

    if (!world_->NavigateToFsArea(fsNodePath)) {
        return false;
    }

    // TODO debug - spawn near staircase!! & remove this stair debug crap
    auto center = 0.5f * sf::Vector2f(
        BaseTile::TileSize.x * GetWorldArea()->GetWidth(),
        BaseTile::TileSize.y * GetWorldArea()->GetHeight()
        );

    if (!GetWorldArea()->GetRelatedNode()->IsRootDirectoryNode()) {
        auto upstair = GetWorldArea()->EmplaceEntity<UpStairEntity>();
        GetWorldArea()->GetEntity<WorldEntity>(upstair)->SetPosition(center);
    }

    std::string downstairTarget;
    auto child = GetWorldArea()->GetRelatedNode()->GetChildNode(0);
    if (child) {
        downstairTarget = child->GetName();
    }

    auto downstair = GetWorldArea()->EmplaceEntity<DownStairEntity>(downstairTarget);
    GetWorldArea()->GetEntity<WorldEntity>(downstair)->SetPosition(center + sf::Vector2f(32.0f, 0.0f));

    if (!SpawnPlayer(center)) {
        return false;
    }

    return true;
}


bool Game::NewGame()
{
    GameFilesystemGen gen;
    if (!(worldFs_ = std::move(gen.GenerateNewFilesystem()))) {
        return false;
    }

    world_ = std::make_unique<World>(*worldFs_);

    if (!ChangeLevel("/")) {
        return false;
    }

    return true;
}


void Game::ViewFollowPlayer()
{
    auto area = GetWorldArea();

    if (area && playerId_ != Entity::InvalidId) {
        area->GetRenderView().setSize(250.0f, 187.5f);
        area->CenterViewOnWorldEntity(playerId_);
    }
}


void Game::Tick()
{
    if (world_) {
        // check if we have a scheduled level change
        if (!scheduledLevelChangeFsNodePath_.empty()) {
            ChangeLevel(scheduledLevelChangeFsNodePath_);
            scheduledLevelChangeFsNodePath_.clear();
        }

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


void Game::RenderUILocation(sf::RenderTarget& target)
{
    if (world_) {
        sf::Text locationText("Location: " + world_->GetCurrentAreaFsPath(), GameAssets::Get().gameFont, 28);
        locationText.setPosition(5.0f, 5.0f);
        locationText.setColor(sf::Color(255, 255, 255));

        Helper::RenderTextWithDropShadow(target, locationText);
    }
}


void Game::RenderUIPlayerUseTargetText(sf::RenderTarget& target)
{
    auto area = GetWorldArea();
    auto player = GetPlayerEntity();

    if (area && player) {
        auto useable = dynamic_cast<IPlayerUsable*>(area->GetEntity(player->GetTargettedUsableEntity()));

        if (useable) {
            sf::Text targetUseText(std::string("Press E: ") + useable->GetUseText(), GameAssets::Get().gameFont, 22);
            targetUseText.setPosition(5.0f, 50.0f);
            targetUseText.setColor(sf::Color(255, 255, 0));

            Helper::RenderTextWithDropShadow(target, targetUseText);
        }
    }
}


void Game::Render(sf::RenderTarget& target)
{
    target.clear();

    if (world_) {
        world_->Render(target);

        RenderUILocation(target);
        RenderUIPlayerUseTargetText(target);
    }
}


void Game::RunFrame(sf::RenderTarget& target)
{
    Tick();
    Render(target);
    
    eventKeysPressed_.clear();
}