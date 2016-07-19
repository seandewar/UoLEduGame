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


bool Game::SpawnPlayer(const sf::Vector2f* optionalStartPos)
{
    auto area = GetWorldArea();

    if (!area) {
        return false;
    }

    playerId_ = area->EmplaceEntity<PlayerEntity>();
    if (playerId_ == Entity::InvalidId) {
        return false;
    }

    // get spawned player & restore stats to our cached copy
    auto player = GetPlayerEntity();
    player->SetStats(cachedPlayerStats_);
    
    // set player location to a default start or start pos if provided
    if (!optionalStartPos) {
        printf("Spawning player at default start location for area\n");
        player->SetPositionToDefaultStart();
    }
    else {
        printf("Spawning player at custom start location\n");
        player->SetPosition(*optionalStartPos);
    }

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
    // try to preload the area
    if (!world_ || !world_->PreloadFsArea(fsNodePath)) {
        return false;
    }

    auto oldArea = GetWorldArea();
    auto oldPlayer = oldArea ? GetPlayerEntity() : nullptr;

    // remove the old player ent and cache it's stats so we can restore them
    // on the new player ent
    if (oldPlayer) {
        cachedPlayerStats_ = oldPlayer->GetStats();
        RemovePlayer();
    }

    // get the old fs node so we can get enough info on where to spawn the player
    // in the new level
    auto oldFsNode = oldArea ? oldArea->GetRelatedNode() : nullptr;

    // try to change level area
    if (!world_->NavigateToFsArea(fsNodePath)) {
        return false;
    }

    ///// TODO DEBUG CRAP - MAKE DUNGEON GEN SPAWN THESE STAIRS /////
    auto center = 0.5f * sf::Vector2f(
        BaseTile::TileSize.x * GetWorldArea()->GetWidth(),
        BaseTile::TileSize.y * GetWorldArea()->GetHeight()
        );

    auto defaultStart = GetWorldArea()->EmplaceEntity<PlayerDefaultStartEntity>();
    GetWorldArea()->GetEntity<WorldEntity>(defaultStart)->SetPosition(center);

    if (GetWorldArea()->GetRelatedNode()->GetParent()) {
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
    ///// TODO END OF DEBUG CRAP /////

    auto currentArea = GetWorldArea();
    auto currentFsNode = currentArea->GetRelatedNode();

    // determine start pos for player based on old fs node
    if (oldFsNode && oldFsNode->GetParent() == currentFsNode) {
        // if our current node is the parent of our old node, spawn at
        // the down stairs that navigate to our old node's area

        auto downstairEnts = currentArea->GetAllEntitiesOfType<DownStairEntity>();
        DownStairEntity* startDownstairEnt = nullptr;

        for (auto entId : downstairEnts) {
            auto ent = currentArea->GetEntity<DownStairEntity>(entId);

            if (ent && ent->GetDestinationFsNodeName() == oldFsNode->GetName()) {
                startDownstairEnt = ent;
                break;
            }
        }

        if (!startDownstairEnt) {
            fprintf(stderr, "WARN - could not find corrisponding DownStairEntity to spawn player at!\n");
            fprintf(stderr, " Will spawn at default start instead.\n");
            fprintf(stderr, "\t(From '%s' to '%s')\n", GameFilesystem::GetNodePathString(*oldFsNode).c_str(),
                GameFilesystem::GetNodePathString(*currentFsNode).c_str());
        }

        if (!SpawnPlayer(startDownstairEnt ? &startDownstairEnt->GetPosition() : nullptr)) {
            return false;
        }
    }
    else if (currentFsNode && currentFsNode->GetParent() == oldFsNode) {
        // if our current node is the child of our old node, then spawn
        // at the up stairs

        auto upstairEnt = currentArea->GetEntity<UpStairEntity>(
            currentArea->GetFirstEntityOfType<UpStairEntity>());

        if (!SpawnPlayer(upstairEnt ? &upstairEnt->GetPosition() : nullptr)) {
            return false;
        }
    }
    else {
        // none of our start spawn conditions were met, just do a default spawn
        if (!SpawnPlayer()) {
            return false;
        }
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