#include "Game.h"

#include <SFML/Graphics/Sprite.hpp>
#include <SFML/Graphics/RectangleShape.hpp>

#include "Helper.h"
#include "GameFilesystemGen.h"
#include "Player.h"
#include "IPlayerUsable.h"
#include "Stairs.h"


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
    LOAD_FROM_FILE(chestsSpriteSheet, "assets/ChestSprites.png");
    LOAD_FROM_FILE(itemsSpriteSheet, "assets/ItemSprites.png");
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


void Game::ResetPlayerStats()
{
    playerStats_.SetMaxHealth(1000);
    playerStats_.SetHealth(1000);

    playerStats_.SetMaxMana(1000);
    playerStats_.SetMana(1000);

    playerStats_.SetMoveSpeed(65.0f);
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
    player->SetStats(&playerStats_);
    player->SetInventory(&playerInv_);
    
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

    // remove the old player ent
    if (oldPlayer) {
        RemovePlayer();
    }

    // get the old fs node so we can get enough info on where to spawn the player
    // in the new level
    auto oldFsNode = oldArea ? oldArea->GetRelatedNode() : nullptr;

    // try to change level area
    if (!world_->NavigateToFsArea(fsNodePath)) {
        return false;
    }

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

    ResetPlayerStats();
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
        sf::Text locationText("Location: " + world_->GetCurrentAreaFsPath(), GameAssets::Get().gameFont, 22);
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
            sf::Text targetUseText(std::string("Press E: ") + useable->GetUseText(), GameAssets::Get().gameFont, 16);
            targetUseText.setPosition(
                sf::Vector2f(target.getView().getCenter().x, 0.8f * target.getView().getSize().y) -
                0.5f * sf::Vector2f(targetUseText.getGlobalBounds().width, targetUseText.getGlobalBounds().height)
                );
            targetUseText.setColor(sf::Color(255, 255, 0));

            Helper::RenderTextWithDropShadow(target, targetUseText);
        }
    }
}


void Game::RenderUIPlayerStats(sf::RenderTarget& target)
{
    auto player = GetPlayerEntity();

    if (player && player->GetStats()) {
        // render health bar bg
        sf::RectangleShape healthBarBg(sf::Vector2f(300.0f, 20.0f));
        healthBarBg.setFillColor(sf::Color(80, 80, 80));
        healthBarBg.setPosition(target.getView().getSize().x - healthBarBg.getSize().x - 5.0f,
            target.getView().getSize().y - 32.5f - healthBarBg.getSize().y);
        healthBarBg.setOutlineThickness(2.0f);
        healthBarBg.setOutlineColor(sf::Color(0, 0, 0));

        target.draw(healthBarBg);

        // render health bar fg
        auto playerHealth = player->GetStats()->GetHealth();
        auto playerMaxHealth = player->GetStats()->GetMaxHealth();

        if (playerMaxHealth > 0) {
            auto healthBarXSizeMul = std::min(1.0f, std::max(0.0f, static_cast<float>(playerHealth) / playerMaxHealth));

            sf::RectangleShape healthBarFg(sf::Vector2f(300.0f * healthBarXSizeMul, 20.0f));
            healthBarFg.setFillColor(sf::Color(255, 0, 0));
            healthBarFg.setPosition(healthBarBg.getPosition());

            target.draw(healthBarFg);
        }

        // render health text label
        sf::Text healthLabelText(std::string("H: ") + std::to_string(playerHealth), GameAssets::Get().gameFont, 16);
        healthLabelText.setPosition(healthBarBg.getPosition() + 0.5f * healthBarBg.getSize() - 0.5f *
            sf::Vector2f(healthLabelText.getGlobalBounds().width, healthLabelText.getGlobalBounds().height));
        healthLabelText.setColor(sf::Color(255, 255, 255));

        Helper::RenderTextWithDropShadow(target, healthLabelText);

        // render mana bar bg
        sf::RectangleShape manaBarBg(sf::Vector2f(300.0f, 20.0f));
        manaBarBg.setFillColor(sf::Color(80, 80, 80));
        manaBarBg.setPosition(target.getView().getSize().x - manaBarBg.getSize().x - 5.0f,
            target.getView().getSize().y - 5.0f - manaBarBg.getSize().y);
        manaBarBg.setOutlineThickness(2.0f);
        manaBarBg.setOutlineColor(sf::Color(0, 0, 0));

        target.draw(manaBarBg);

        // render mana bar fg
        auto playerMana = player->GetStats()->GetMana();
        auto playerMaxMana = player->GetStats()->GetMaxMana();

        if (playerMaxMana > 0) {
            auto manaBarXSizeMul = std::min(1.0f, std::max(0.0f, static_cast<float>(playerMana) / playerMaxMana));

            sf::RectangleShape manaBarFg(sf::Vector2f(300.0f * manaBarXSizeMul, 20.0f));
            manaBarFg.setFillColor(sf::Color(0, 0, 255));
            manaBarFg.setPosition(manaBarBg.getPosition());

            target.draw(manaBarFg);
        }

        // render mana text label
        sf::Text manaLabelText(std::string("M: ") + std::to_string(playerMana), GameAssets::Get().gameFont, 16);
        manaLabelText.setPosition(manaBarBg.getPosition() + 0.5f * manaBarBg.getSize() - 0.5f *
            sf::Vector2f(manaLabelText.getGlobalBounds().width, manaLabelText.getGlobalBounds().height));
        manaLabelText.setColor(sf::Color(255, 255, 255));

        Helper::RenderTextWithDropShadow(target, manaLabelText);
    }
}


void Game::RenderUILoadingArea(sf::RenderTarget& target)
{
    sf::Text loadingText("Loading Area...", GameAssets::Get().gameFont, 16);
    loadingText.setPosition(target.getView().getCenter() - 0.5f *
        sf::Vector2f(loadingText.getGlobalBounds().width, loadingText.getGlobalBounds().height));

    Helper::RenderTextWithDropShadow(target, loadingText);
}


void Game::RenderUIItem(sf::RenderTarget& target, const sf::Vector2f& position, const std::string& label,
    Item* item, bool isHighlighted)
{
    // render inv item bg and border
    sf::RectangleShape invItemBg(sf::Vector2f(50.0f, 50.0f));
    invItemBg.setFillColor(isHighlighted ? sf::Color(40, 40, 40, 215) : sf::Color(20, 20, 20, 215));
    invItemBg.setOutlineColor(isHighlighted ? sf::Color(255, 255, 255) : sf::Color(0, 0, 0));
    invItemBg.setOutlineThickness(2.0f);
    invItemBg.setPosition(position);

    target.draw(invItemBg);

    // render inv item sprite
    if (item && item->GetAmount() > 0) {
        auto invItemSprite = item->GetSprite();
        invItemSprite.setScale(3.0f, 3.0f);
        invItemSprite.setPosition(invItemBg.getPosition());

        target.draw(invItemSprite);

        // render item amount
        if (item->GetAmount() > 1) {
            sf::Text invItemAmountText(std::to_string(item->GetAmount()), GameAssets::Get().gameFont, 8);
            invItemAmountText.setPosition(
                invItemBg.getPosition().x + invItemBg.getSize().x - invItemAmountText.getGlobalBounds().width - 2.0f,
                invItemBg.getPosition().y + 2.0f
                );
            invItemAmountText.setColor(sf::Color(255, 255, 0));

            Helper::RenderTextWithDropShadow(target, invItemAmountText);
        }
    }

    // render inv text label
    sf::Text invItemKeyText(label, GameAssets::Get().gameFont, 8);
    invItemKeyText.setPosition(invItemBg.getPosition() + sf::Vector2f(2.0f, 2.0f));
    invItemKeyText.setColor(sf::Color(255, 255, 255));

    Helper::RenderTextWithDropShadow(target, invItemKeyText);
}


void Game::RenderUIPlayerInventory(sf::RenderTarget& target)
{
    auto player = GetPlayerEntity();

    if (player) {
        auto playerInv = player->GetInventory();

        if (playerInv) {
            // render weapons inv text label
            sf::Text invWeaponsLabelText("Weapons", GameAssets::Get().gameFont, 8);
            invWeaponsLabelText.setPosition(5.0f,
                target.getView().getSize().y - 60.0f - invWeaponsLabelText.getGlobalBounds().height);
            invWeaponsLabelText.setColor(sf::Color(255, 255, 255));

            Helper::RenderTextWithDropShadow(target, invWeaponsLabelText);

            // TODO render melee wep item
            RenderUIItem(target, sf::Vector2f(5.0f, target.getView().getSize().y - 55.0f),
                "1", nullptr, false);

            // TODO render magic wep item
            RenderUIItem(target, sf::Vector2f(62.5f, target.getView().getSize().y - 55.0f),
                "2", nullptr);

            // render potions inv text label
            sf::Text invPotionsLabelText("Potions", GameAssets::Get().gameFont, 8);
            invPotionsLabelText.setPosition(122.5f,
                target.getView().getSize().y - 60.0f - invPotionsLabelText.getGlobalBounds().height);
            invPotionsLabelText.setColor(sf::Color(255, 255, 255));

            Helper::RenderTextWithDropShadow(target, invPotionsLabelText);

            // render health potion item
            RenderUIItem(target, sf::Vector2f(122.5f, target.getView().getSize().y - 55.0f),
                "3", playerInv->GetHealthPotions());

            // render magic potion item
            RenderUIItem(target, sf::Vector2f(180.0f, target.getView().getSize().y - 55.0f),
                "4", playerInv->GetMagicPotions());

            // render special inv text label
            sf::Text invSpecLabelText("Special", GameAssets::Get().gameFont, 8);
            invSpecLabelText.setPosition(241.0f,
                target.getView().getSize().y - 60.0f - invSpecLabelText.getGlobalBounds().height);
            invSpecLabelText.setColor(sf::Color(255, 255, 255));

            Helper::RenderTextWithDropShadow(target, invSpecLabelText);

            // TODO render special item
            RenderUIItem(target, sf::Vector2f(241.0f, target.getView().getSize().y - 55.0f),
                "5", nullptr);
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
        RenderUIPlayerStats(target);
        RenderUIPlayerInventory(target);

        // if level is going to change next frame, display loading text
        if (!scheduledLevelChangeFsNodePath_.empty()) {
            RenderUILoadingArea(target);
        }
    }
}


void Game::RunFrame(sf::RenderTarget& target)
{
    Tick();
    Render(target);
    
    eventKeysPressed_.clear();
}