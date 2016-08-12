#include "Game.h"

#include <SFML/Window/Mouse.hpp>
#include <SFML/Graphics/Sprite.hpp>
#include <SFML/Graphics/RectangleShape.hpp>

#include "Helper.h"
#include "GameFilesystemGen.h"
#include "Player.h"
#include "PlayerUsable.h"
#include "Stairs.h"
#include "Chest.h"


#define LOAD_FROM_FILE(asset, assetPath) \
    printf("Loading asset " #assetPath "..\n"); \
    if (!asset.loadFromFile(assetPath)) { \
        fprintf(stderr, "Failed to load asset " #assetPath "!\n"); \
        return false; \
    }


bool GameAssets::LoadAssets()
{
    printf("Loading game assets...\n");

    // fonts
    LOAD_FROM_FILE(gameFont, "assets/Fonts/PressStart2P.ttf");
    LOAD_FROM_FILE(altFont, "assets/Fonts/prstart.ttf");

    // textures
    LOAD_FROM_FILE(viewVignette, "assets/Textures/DarknessVignette.png");
    LOAD_FROM_FILE(genericTilesSheet, "assets/Textures/GenericTiles.png");
    LOAD_FROM_FILE(stairsSpriteSheet, "assets/Textures/StairSprites.png");
    LOAD_FROM_FILE(playerSpriteSheet, "assets/Textures/PlayerSprites.png");
    LOAD_FROM_FILE(chestsSpriteSheet, "assets/Textures/ChestSprites.png");
    LOAD_FROM_FILE(itemsSpriteSheet, "assets/Textures/ItemSprites.png");
    LOAD_FROM_FILE(altarSpriteSheet, "assets/Textures/AltarSprites.png");
    LOAD_FROM_FILE(enemySpriteSheet, "assets/Textures/EnemySprites.png");
    LOAD_FROM_FILE(damageTypesSpriteSheet, "assets/Textures/DamageTypeSprites.png");
    LOAD_FROM_FILE(effectSpriteSheet, "assets/Textures/EffectSprites.png");
    LOAD_FROM_FILE(sparkleSpriteSheet, "assets/Textures/SparkleSprites.png");
    LOAD_FROM_FILE(projectileSpriteSheet, "assets/Textures/ProjectileSprites.png");

    // sound buffers
    LOAD_FROM_FILE(drinkSoundBuffer, "assets/Sounds/DrinkSound.wav");
    LOAD_FROM_FILE(blastSoundBuffer, "assets/Sounds/BlastSound.wav");
    LOAD_FROM_FILE(waveSoundBuffer, "assets/Sounds/WaveSound.wav");
    LOAD_FROM_FILE(drainSoundBuffer, "assets/Sounds/DrainSound.wav");
    LOAD_FROM_FILE(zeroBlastSoundBuffer, "assets/Sounds/ZeroBlastSound.wav");
    LOAD_FROM_FILE(hitSoundBuffer, "assets/Sounds/HitSound.wav");
    LOAD_FROM_FILE(specSoundBuffer, "assets/Sounds/SpecSound.wav");
    LOAD_FROM_FILE(lowHealthSoundBuffer, "assets/Sounds/LowHealthSound.wav");
    LOAD_FROM_FILE(deathSoundBuffer, "assets/Sounds/DeathSound.wav");
    LOAD_FROM_FILE(playerHurtSoundBuffer, "assets/Sounds/PlayerHurtSound.wav");
    LOAD_FROM_FILE(playerDeathSoundBuffer, "assets/Sounds/PlayerDeathSound.wav");
    LOAD_FROM_FILE(pickupSoundBuffer, "assets/Sounds/PickupSound.wav");
    LOAD_FROM_FILE(pickup2SoundBuffer, "assets/Sounds/Pickup2Sound.wav");
    LOAD_FROM_FILE(selectSoundBuffer, "assets/Sounds/SelectSound.wav");
    LOAD_FROM_FILE(attackSoundBuffer, "assets/Sounds/AttackSound.wav");
    LOAD_FROM_FILE(successSoundBuffer, "assets/Sounds/SuccessSound.wav");
    LOAD_FROM_FILE(failureSoundBuffer, "assets/Sounds/FailureSound.wav");
    LOAD_FROM_FILE(openChestSoundBuffer, "assets/Sounds/OpenChestSound.wav");
    LOAD_FROM_FILE(blockSoundBuffer, "assets/Sounds/BlockSound.wav");
    LOAD_FROM_FILE(armourPenSoundBuffer, "assets/Sounds/ArmourPenSound.wav");
    LOAD_FROM_FILE(invincibilitySoundBuffer, "assets/Sounds/InvincibilitySound.wav");
    LOAD_FROM_FILE(magicFireSoundBuffer, "assets/Sounds/MagicFireSound.wav");
    LOAD_FROM_FILE(smokeSoundBuffer, "assets/Sounds/SmokeSound.wav");
    LOAD_FROM_FILE(bossSwordSoundBuffer, "assets/Sounds/BossSwordSound.wav");
    LOAD_FROM_FILE(bossSpawnSoundBuffer, "assets/Sounds/BossSpawnSound.wav");
    LOAD_FROM_FILE(bossDyingSoundBuffer, "assets/Sounds/BossDyingSound.wav");
    LOAD_FROM_FILE(bossDeadSoundBuffer, "assets/Sounds/BossDeadSound.wav");
    LOAD_FROM_FILE(bossActionSoundBuffer, "assets/Sounds/BossActionSound.wav");

    // sounds init
    selectSound.setBuffer(selectSoundBuffer);
    drinkSound.setBuffer(drinkSoundBuffer);
    blastSound.setBuffer(blastSoundBuffer);
    waveSound.setBuffer(waveSoundBuffer);
    drainSound.setBuffer(drainSoundBuffer);
    zeroBlastSound.setBuffer(zeroBlastSoundBuffer);
    pickupSound.setBuffer(pickupSoundBuffer);
    artefactPickupSound.setBuffer(pickup2SoundBuffer);
    attackSound.setBuffer(attackSoundBuffer);
    hitSound.setBuffer(hitSoundBuffer);
    specSound.setBuffer(specSoundBuffer);
    lowHealthSound.setBuffer(lowHealthSoundBuffer);
    deathSound.setBuffer(deathSoundBuffer);
    playerHurtSound.setBuffer(playerHurtSoundBuffer);
    playerDeathSound.setBuffer(playerDeathSoundBuffer);
    successSound.setBuffer(successSoundBuffer);
    failureSound.setBuffer(failureSoundBuffer);
    openChestSound.setBuffer(openChestSoundBuffer);
    blockSound.setBuffer(blockSoundBuffer);
    armourPenSound.setBuffer(armourPenSoundBuffer);
    invincibilitySound.setBuffer(invincibilitySoundBuffer);
    magicFireSound.setBuffer(magicFireSoundBuffer);
    smokeSound.setBuffer(smokeSoundBuffer);
    bossSwordSound.setBuffer(bossSwordSoundBuffer);
    bossSpawnSound.setBuffer(bossSpawnSoundBuffer);
    bossDyingSound.setBuffer(bossDyingSoundBuffer);
    bossDeadSound.setBuffer(bossDeadSoundBuffer);
    bossActionSound.setBuffer(bossActionSoundBuffer);

    return true;
}


const sf::Time Game::FrameTimeStep = sf::microseconds(16667);


Game::Game() :
debugMode_(false),
playerId_(Entity::InvalidId),
state_(GameState::Menu1),
mapMode_(false)
{
    // add these so that they can be shuffled when the display question UI is invoked.
    displayedQuestionShuffledChoices_.emplace_back(GameQuestionAnswerChoice::CorrectChoice);
    displayedQuestionShuffledChoices_.emplace_back(GameQuestionAnswerChoice::WrongChoice1);
    displayedQuestionShuffledChoices_.emplace_back(GameQuestionAnswerChoice::WrongChoice2);

    ResetDisplayedQuestion();
}


Game::~Game()
{
}


bool Game::Init()
{
    GameFilesystemGen gen;
    if (!(worldFs_ = std::move(gen.GenerateNewFilesystem()))) {
        return false;
    }

    world_ = std::make_unique<World>(*worldFs_);
    return world_->NavigateToFsArea("/");
}


void Game::ResetDisplayedQuestion()
{
    displayedQuestion_ = nullptr;
    displayedQuestionSelectedChoice_ = 0;
}


void Game::ResetPlayerStats()
{
    playerStats_.SetMaxHealth(1000);
    playerStats_.SetHealth(1000);

    playerStats_.SetMaxMana(1000);
    playerStats_.SetMana(1000);

    playerStats_.SetMoveSpeed(75.0f);
}


bool Game::SpawnPlayer(const sf::Vector2f* optionalStartPos)
{
    auto area = GetWorldArea();

    if (!area) {
        return false;
    }

    if (playerId_ != Entity::InvalidId && !RemovePlayer()) {
        printf("SpawnPlayer() - could not remove previous player!\n");
        return false;
    }

    playerId_ = area->EmplaceEntity<PlayerEntity>();
    if (playerId_ == Entity::InvalidId) {
        return false;
    }

    // get spawned player, set stats & restore invinc time left
    auto player = GetPlayerEntity();
    player->SetStats(&playerStats_);
    player->SetInventory(&playerInv_);
    player->SetInvincibility(playerCachedInvincTimeLeft_);
    
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
    auto player = GetPlayerEntity();

    if (!GetWorldArea() || !player) {
        playerId_ = Entity::InvalidId;
        return false;
    }

    // cache invincibility time that was left so we can restore it
    playerCachedInvincTimeLeft_ = player->GetInvincibilityTimeLeft();

    player->MarkForDeletion();
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

    ResetDisplayedQuestion();
    director_.PlayerChangedArea(currentArea);
    mapMode_ = false;

    GameAssets::Get().openChestSound.play();
    AddMessage("You are on floor " + GameFilesystem::GetNodePathString(*currentFsNode),
        sf::Color(255, 255, 0));
    return true;
}


bool Game::TeleportPlayerToObjective()
{
    switch (director_.GetCurrentObjectiveType()) {
    default:
        return true;

    case GameObjectiveType::CollectArtefact:
        if (director_.GetCurrentArtefactNode()) {
            auto levelNode = director_.GetCurrentArtefactNode()->GetParent();

            if (!levelNode) {
                return false;
            }

            if (!ChangeLevel(GameFilesystem::GetNodePathString(*levelNode)) || !GetWorldArea()) {
                return false;
            }

            auto chestIds = GetWorldArea()->GetAllEntitiesOfType<ChestEntity>();

            for (auto id : chestIds) {
                auto chestEnt = GetWorldArea()->GetEntity<ChestEntity>(id);
                assert(chestEnt);

                if (director_.GetCurrentArtefactNode()->GetName() == chestEnt->GetChestFsNodeName()) {
                    // tp player to artefact chest
                    if (GetPlayerEntity()) {
                        printf("Teleported player to artefact chest.\n");
                        GetPlayerEntity()->SetCenterPosition(chestEnt->GetCenterPosition());
                    }

                    break;
                }
            }
        }
        return true;

    case GameObjectiveType::RootArtefactAltar:
        if (!ChangeLevel("/")) {
            return false;
        }

        if (GetPlayerEntity()) {
            printf("Teleported player to default start in root.\n");
            return GetPlayerEntity()->SetPositionToDefaultStart();
        }

        return true;
    }
}


bool Game::NewGame()
{
    messages_.clear();

    GameFilesystemGen gen;
    if (!(worldFs_ = std::move(gen.GenerateNewFilesystem()))) {
        return false;
    }

    AddMessage("Welcome to the File System Dungeon!", sf::Color(173, 216, 230));
    AddMessage("The seed for this dungeon is " + std::to_string(gen.GetSeed()) + ".", sf::Color(173, 216, 230));

    world_ = std::make_unique<World>(*worldFs_);
    director_.StartNewSession(9, nullptr, worldFs_.get());

    ResetDisplayedQuestion();

    playerCachedInvincTimeLeft_ = sf::seconds(3.0f); // give player some spawn invincibility
    if (!ChangeLevel("/")) {
        return false;
    }

    ResetPlayerStats();
    state_ = GameState::InGame;
    mapMode_ = false;
    return true;
}


void Game::UpdateCamera(sf::RenderTarget& target)
{
    auto area = GetWorldArea();

    if (area) {
        if (state_ == GameState::Menu1) {
            area->GetRenderView().setSize(Helper::ComputeGoodAspectSize(target, 1000.0f));
            area->GetRenderView().setCenter(0.5f * area->GetWidth() * BaseTile::TileSize.x,
                0.5f * area->GetHeight() * BaseTile::TileSize.y);
        }
        else if (state_ == GameState::InGame) {
            if (mapMode_) {
                // zoomed out for map mode
                area->GetRenderView().setSize(Helper::ComputeGoodAspectSize(target, 1400.0f));
            }
            else {
                area->GetRenderView().setSize(Helper::ComputeGoodAspectSize(target, 350.0f));
            }

            if (playerId_ != Entity::InvalidId) {
                area->CenterViewOnWorldEntity(playerId_);
            }
        }
    }
}


void Game::SetDisplayedQuestion(const IGameQuestion* question)
{
    displayedQuestion_ = question;
    displayedQuestionSelectedChoice_ = 0;

    // shuffle choices
    std::random_shuffle(displayedQuestionShuffledChoices_.begin(), displayedQuestionShuffledChoices_.end());
}


void Game::HandleDisplayedQuestionInput()
{
    if (!displayedQuestion_ && GetWorldArea()) {
        return;
    }

    // ESC to close
    if (Game::IsKeyPressedFromEvent(sf::Keyboard::Escape)) {
        ResetDisplayedQuestion();
        return;
    }

    // arrow keys
    bool changedInput = false;
    if (Game::IsKeyPressedFromEvent(sf::Keyboard::Up)) {
        if (displayedQuestionSelectedChoice_-- <= 0) {
            displayedQuestionSelectedChoice_ = 2;
        }

        changedInput = true;
    }
    else if (Game::IsKeyPressedFromEvent(sf::Keyboard::Down)) {
        if (++displayedQuestionSelectedChoice_ > 2) {
            displayedQuestionSelectedChoice_ = 0;
        }

        changedInput = true;
    }

    // number keys
    if (Game::IsKeyPressedFromEvent(sf::Keyboard::Num1)) {
        displayedQuestionSelectedChoice_ = 0;
        changedInput = true;
    }
    else if (Game::IsKeyPressedFromEvent(sf::Keyboard::Num2)) {
        displayedQuestionSelectedChoice_ = 1;
        changedInput = true;
    }
    else if (Game::IsKeyPressedFromEvent(sf::Keyboard::Num3)) {
        displayedQuestionSelectedChoice_ = 2;
        changedInput = true;
    }

    // play changed input sound
    if (changedInput) {
        GameAssets::Get().selectSound.play();
    }

    // ENTER to select
    if (Game::IsKeyPressedFromEvent(sf::Keyboard::Return)) {
        auto selectedChoice = displayedQuestionShuffledChoices_[displayedQuestionSelectedChoice_];
        
        if (selectedChoice == GameQuestionAnswerChoice::CorrectChoice) {
            GameAssets::Get().successSound.play();
            director_.AnswerQuestionResult(GameQuestionAnswerResult::Correct, GetWorldArea());
        }
        else {
            GameAssets::Get().failureSound.play();
            director_.AnswerQuestionResult(GameQuestionAnswerResult::Wrong, GetWorldArea());
        }

        ResetDisplayedQuestion();
        return;
    }
}


void Game::HandleRespawnSacrificeInput()
{
    auto player = GetPlayerEntity();

    if (!player) {
        return;
    }

    auto pstats = player->GetStats();

    // if our player is alive, don't bother
    if (!pstats || pstats->IsAlive()) {
        return;
    }

    if (Game::IsKeyPressedFromEvent(sf::Keyboard::Return)) {
        AddMessage("You have been revived!", sf::Color(0, 255, 0));
        GameAssets::Get().selectSound.play();
        GameAssets::Get().invincibilitySound.play();

        auto pInv = player->GetInventory();

        // remove items
        if (pInv) {
            if (pInv->GetMagicWeapon() && pInv->GetMagicWeapon()->GetAmount() > 0) {
                // magic weapon
                AddMessage("You lose your " + pInv->GetMagicWeapon()->GetItemName(), sf::Color(100, 0, 0));
                pInv->GetMagicWeapon()->SetAmount(0);
            }

            if (pInv->GetHealthPotions() && pInv->GetHealthPotions()->GetAmount() > 0) {
                // health potions
                AddMessage("You lose your Health Potions", sf::Color(100, 0, 0));
                pInv->GetHealthPotions()->SetAmount(0);
            }

            if (pInv->GetMagicPotions() && pInv->GetMagicPotions()->GetAmount() > 0) {
                // magic potions
                AddMessage("You lose your Magic Potions", sf::Color(100, 0, 0));
                pInv->GetMagicPotions()->SetAmount(0);
            }
        }

        // remove remaining mana
        if (pstats->GetMana() > 0) {
            AddMessage("You lose your remaining Mana", sf::Color(100, 0, 0));
            pstats->SetMana(0);
        }

        pstats->SetHealth(pstats->GetMaxHealth()); // restore health for respawn
        player->SetInvincibility(sf::seconds(5.0f)); // invincibility period so we're not insta dead again
        mapMode_ = false; // force player out of map mode... maybe for convienience...?
    }
}


void Game::HandleUseInventory()
{
    auto player = GetPlayerEntity();

    if (!player) {
        return;
    }

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Num1) || sf::Mouse::isButtonPressed(sf::Mouse::Left)) {
        player->UseInventorySlot(PlayerInventorySlot::MeleeWeapon);
    }
    else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Num2) || sf::Mouse::isButtonPressed(sf::Mouse::Right)) {
        player->UseInventorySlot(PlayerInventorySlot::MagicWeapon);
    }
    
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Num3)) {
        player->UseInventorySlot(PlayerInventorySlot::HealthPotions);
    }
    else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Num4)) {
        player->UseInventorySlot(PlayerInventorySlot::MagicPotions);
    }
}


void Game::HandlePlayerMoveInput()
{
    auto player = GetPlayerEntity();

    if (!player) {
        return;
    }

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::W)) {
        player->AddMoveInDirection(PlayerFacingDirection::Up);
    }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::S)) {
        player->AddMoveInDirection(PlayerFacingDirection::Down);
    }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::A)) {
        player->AddMoveInDirection(PlayerFacingDirection::Left);
    }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::D)) {
        player->AddMoveInDirection(PlayerFacingDirection::Right);
    }
}


void Game::HandlePlayerLowHealthBeep()
{
    auto player = GetPlayerEntity();

    if (!player || !player->GetStats()) {
        return;
    }

    lowHealthNextBeepTimeLeft_ -= FrameTimeStep;

    // beep if low health and not dead if timer is <= 0
    if (IsPlayerLowHealth() && player->GetStats()->IsAlive()) {
        if (lowHealthNextBeepTimeLeft_ <= sf::Time::Zero) {
            GameAssets::Get().lowHealthSound.play();

            lowHealthNextBeepTimeLeft_ = sf::seconds(1.0f);
        }
    }
    else {
        // immediately beep next time we hit the condition
        lowHealthNextBeepTimeLeft_ = sf::Time::Zero;
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

        auto player = GetPlayerEntity();

        // debug mode
        if (debugMode_) {
            if (Game::IsKeyPressedFromEvent(sf::Keyboard::F2)) {
                // tp to objective
                TeleportPlayerToObjective();
            }

            if (player) {
                // give invincibility if debug mode
                player->SetInvincibility(sf::seconds(2.0f));

                // kill / revive player
                auto pstats = player->GetStats();

                if (pstats) {
                    // toggle player killed/alive
                    if (Game::IsKeyPressedFromEvent(sf::Keyboard::K)) {
                        if (pstats->IsAlive()) {
                            player->DamageWithoutInvincibility(pstats->GetHealth());
                        }
                        else {
                            pstats->SetHealth(pstats->GetMaxHealth());
                        }
                    }

                    // restore hp and mana if in debug mode (if not dead)
                    if (pstats->IsAlive()) {
                        pstats->SetHealth(pstats->GetMaxHealth());
                        pstats->SetMana(pstats->GetMaxMana());
                    }
                }

                // give weapons & armour
                std::unique_ptr<BaseWeaponItem> weapon;
                std::unique_ptr<Armour> armour;

                if (Game::IsKeyPressedFromEvent(sf::Keyboard::F3)) {
                    weapon = std::make_unique<MeleeWeapon>(MeleeWeaponType::AdventurerSword);
                }
                else if (Game::IsKeyPressedFromEvent(sf::Keyboard::F4)) {
                    weapon = std::make_unique<MagicWeapon>(MagicWeaponType::FlameStaff);
                }
                else if (Game::IsKeyPressedFromEvent(sf::Keyboard::F5)) {
                    weapon = std::make_unique<MagicWeapon>(MagicWeaponType::WaveStaff);
                }
                else if (Game::IsKeyPressedFromEvent(sf::Keyboard::F6)) {
                    weapon = std::make_unique<MagicWeapon>(MagicWeaponType::DrainStaff);
                }
                else if (Game::IsKeyPressedFromEvent(sf::Keyboard::F7)) {
                    weapon = std::make_unique<MagicWeapon>(MagicWeaponType::InvincibilityStaff);
                }
                else if (Game::IsKeyPressedFromEvent(sf::Keyboard::F8)) {
                    armour = std::make_unique<Armour>(ArmourType::WarriorHelmet);
                }
                else if (Game::IsKeyPressedFromEvent(sf::Keyboard::F9)) {
                    armour = std::make_unique<Armour>(ArmourType::AntiMagicVisor);
                }
                else if (Game::IsKeyPressedFromEvent(sf::Keyboard::F10)) {
                    armour = std::make_unique<Armour>(ArmourType::BalanceHeadgear);
                }
                else if (Game::IsKeyPressedFromEvent(sf::Keyboard::F11) && GetWorldArea()) {
                    // find all artefacts lol
                    for (int i = 0; i < director_.GetMaxArtefacts(); ++i) {
                        director_.FoundArtefact(GetWorldArea());
                    }

                    weapon = std::make_unique<MeleeWeapon>(MeleeWeaponType::ThornedSabre);
                    armour = std::make_unique<Armour>(ArmourType::BalanceHeadgear);
                    player->PickupItem(&PotionItem(ItemType::HealthPotion, 10));
                    player->PickupItem(&PotionItem(ItemType::MagicPotion, 10));
                }

                if (weapon) {
                    weapon->SetDifficultyMultiplier(director_.GetCurrentDifficultyMultiplier());
                    player->PickupItem(weapon.get());
                }

                if (armour) {
                    armour->SetDifficultyMultiplier(director_.GetCurrentDifficultyMultiplier());
                    player->PickupItem(armour.get());
                }
            }
        }

        // handle state specific input
        if (state_ == GameState::InGame) {
            // handle map mode toggle
            if (Game::IsKeyPressedFromEvent(sf::Keyboard::M) || Game::IsKeyPressedFromEvent(sf::Keyboard::Tab)) {
                GameAssets::Get().selectSound.play();
                mapMode_ = !mapMode_;
            }

            // handle low player health beep
            HandlePlayerLowHealthBeep();

            // handle input for player inputs, interfaces or inventory
            HandlePlayerMoveInput();

            if (player && player->GetStats() && !player->GetStats()->IsAlive()) {
                HandleRespawnSacrificeInput();
            }
            else if (displayedQuestion_) {
                HandleDisplayedQuestionInput();
            }
            else {
                HandleUseInventory();
                
                // use ent in player's target
                if (player && IsKeyPressedFromEvent(sf::Keyboard::E)) {
                    player->SetUseTargetThisFrame(true);
                }
            }
        }
        else if (state_ == GameState::Menu1) {
            // enter to start game
            if (Game::IsKeyPressedFromEvent(sf::Keyboard::Return)) {
                GameAssets::Get().successSound.play();

                if (!NewGame()) {
                    throw std::runtime_error("Failed to start new game!");
                }
            }
        }

        world_->SetDebugMode(debugMode_);
        world_->Tick();
    }
}


void Game::RenderUILocation(sf::RenderTarget& target)
{
    if (world_) {
        sf::Text locationText(world_->GetCurrentAreaFsPath(), GameAssets::Get().gameFont, 22);
        locationText.setPosition(5.0f, 5.0f);
        locationText.setColor(sf::Color(255, 255, 255));

        Helper::RenderTextWithDropShadow(target, locationText);
    }
}


void Game::RenderUIObjective(sf::RenderTarget& target)
{
    if (world_ &&
        director_.GetCurrentObjectiveType() != GameObjectiveType::NotStarted &&
        director_.GetCurrentObjectiveType() != GameObjectiveType::Complete) {
        sf::Text objText("Objective: " + director_.GetObjectiveText(), GameAssets::Get().gameFont, 8);
        objText.setPosition(5.0f, 35.0f);
        objText.setColor(sf::Color(255, 150, 0));

        Helper::RenderTextWithDropShadow(target, objText);
    }
}


void Game::RenderUIPlayerUseTargetText(sf::RenderTarget& target)
{
    auto area = GetWorldArea();
    auto player = GetPlayerEntity();

    if (area && player) {
        auto useable = dynamic_cast<PlayerUsable*>(area->GetEntity(player->GetTargettedUsableEntity()));

        if (useable) {
            sf::Text targetUseText(std::string(), GameAssets::Get().gameFont, 16);

            if (useable->IsUsable(player->GetAssignedId())) {
                targetUseText.setString(std::string("Press E: ") + useable->GetUseText());
                targetUseText.setColor(sf::Color(255, 255, 0));
            }
            else {
                // our target is not currently usable right now.
                if (useable->GetCannotUseText().empty()) {
                    return;
                }

                targetUseText.setString(useable->GetCannotUseText());
                targetUseText.setColor(sf::Color(150, 150, 150));
            }

            targetUseText.setPosition(
                sf::Vector2f(target.getView().getCenter().x, 0.15f * target.getView().getSize().y) -
                0.5f * sf::Vector2f(targetUseText.getGlobalBounds().width, targetUseText.getGlobalBounds().height)
                );

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
        healthBarBg.setPosition(target.getView().getSize().x - healthBarBg.getSize().x - 5.0f,
            target.getView().getSize().y - 32.5f - healthBarBg.getSize().y);
        healthBarBg.setOutlineThickness(2.0f);

        // low health flash
        if (IsPlayerLowHealth() && player->GetStats()->IsAlive() &&
            lowHealthNextBeepTimeLeft_ >= sf::seconds(0.75f)) {
            healthBarBg.setOutlineColor(sf::Color(255, 0, 0));
            healthBarBg.setFillColor(sf::Color(50, 20, 20));
        }
        else {
            healthBarBg.setOutlineColor(sf::Color(0, 0, 0));
            healthBarBg.setFillColor(sf::Color(20, 20, 20));
        }

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
        manaBarBg.setFillColor(sf::Color(20, 20, 20));
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


void Game::RenderUIControls(sf::RenderTarget& target)
{
    sf::Text moveControls("Press W, A, S, D to move", GameAssets::Get().gameFont, 8);
    moveControls.setColor(sf::Color(255, 255, 0));
    moveControls.setPosition(target.getSize().x - moveControls.getGlobalBounds().width - 5.0f,
        target.getView().getSize().y - 100.0f);

    Helper::RenderTextWithDropShadow(target, moveControls);

    sf::Text invControls("Press 1-4 to use inventory item", GameAssets::Get().gameFont, 8);
    invControls.setColor(sf::Color(255, 255, 0));
    invControls.setPosition(target.getSize().x - invControls.getGlobalBounds().width - 5.0f,
        target.getView().getSize().y - 85.0f);

    Helper::RenderTextWithDropShadow(target, invControls);

    sf::Text mapControls("Press M or TAB to toggle map mode", GameAssets::Get().gameFont, 8);
    mapControls.setColor(sf::Color(255, 255, 0));
    mapControls.setPosition(target.getSize().x - mapControls.getGlobalBounds().width - 5.0f,
        target.getView().getSize().y - 70.0f);

    Helper::RenderTextWithDropShadow(target, mapControls);
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
        
        if (item->GetUseDelayTimeLeft() > sf::Time::Zero) {
            invItemSprite.setColor(sf::Color(255, 255, 255, 100));
        }

        target.draw(invItemSprite);

        // render item amount
        if (item->GetMaxAmount() > 1) {
            sf::Text invItemAmountText(std::to_string(item->GetAmount()), GameAssets::Get().gameFont, 8);
            invItemAmountText.setPosition(
                invItemBg.getPosition().x + invItemBg.getSize().x - invItemAmountText.getGlobalBounds().width - 2.0f,
                invItemBg.getPosition().y + 2.0f
                );

            invItemAmountText.setColor(item->GetAmount() < item->GetMaxAmount() ? sf::Color(255, 255, 0) : sf::Color(255, 0, 0));

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
                target.getView().getSize().y - 192.0f - invWeaponsLabelText.getGlobalBounds().height);
            invWeaponsLabelText.setColor(sf::Color(255, 255, 255));

            Helper::RenderTextWithDropShadow(target, invWeaponsLabelText);

            // render melee wep item
            RenderUIItem(target, sf::Vector2f(5.0f, target.getView().getSize().y - 187.0f),
                "1", playerInv->GetMeleeWeapon(), playerInv->GetSelectedWeapon() == PlayerSelectedWeapon::Melee);

            // render melee wep info
            // melee icon sprite
            sf::Sprite invMeleeInfoSprite(GameAssets::Get().damageTypesSpriteSheet, sf::IntRect(0, 0, 16, 16));
            invMeleeInfoSprite.setPosition(62.0f, target.getView().getSize().y - 192.0f);

            target.draw(invMeleeInfoSprite);

            // melee name
            sf::Text invMeleeInfoNameText(std::string(), GameAssets::Get().gameFont, 8);
            invMeleeInfoNameText.setPosition(82.0f, target.getView().getSize().y - 187.0f);
            invMeleeInfoNameText.setColor(sf::Color(255, 255, 255));

            if (!playerInv->GetMeleeWeapon() || playerInv->GetMeleeWeapon()->GetAmount() <= 0) {
                invMeleeInfoNameText.setString("None");
            }
            else {
                invMeleeInfoNameText.setString(playerInv->GetMeleeWeapon()->GetItemName());

                // melee stats
                std::ostringstream oss;
                oss << "Attack: " << playerInv->GetMeleeWeapon()->GetAttack() << "\n";
                oss << "Range:  " << std::fixed << std::setprecision(1) <<
                    playerInv->GetMeleeWeapon()->GetAttackRange() << "\n";
                oss << "Delay:  " << std::fixed << std::setprecision(1) <<
                    playerInv->GetMeleeWeapon()->GetUseDelay().asSeconds() << "s";

                sf::Text invMeleeInfoStatsText(oss.str(), GameAssets::Get().gameFont, 8);
                invMeleeInfoStatsText.setPosition(64.0f, target.getView().getSize().y - 174.0f);
                invMeleeInfoStatsText.setColor(sf::Color(255, 255, 255));

                Helper::RenderTextWithDropShadow(target, invMeleeInfoStatsText);

                // melee desc
                sf::Text invMeleeInfoDescText(playerInv->GetMeleeWeapon()->GetShortDescription(),
                    GameAssets::Get().gameFont, 8);
                invMeleeInfoDescText.setPosition(64.0f, target.getView().getSize().y - 144.0f);
                invMeleeInfoDescText.setColor(sf::Color(255, 255, 0));

                Helper::RenderTextWithDropShadow(target, invMeleeInfoDescText);
            }

            Helper::RenderTextWithDropShadow(target, invMeleeInfoNameText);

            // render magic wep item
            RenderUIItem(target, sf::Vector2f(5.0f, target.getView().getSize().y - 126.0f),
                "2", playerInv->GetMagicWeapon(), playerInv->GetSelectedWeapon() == PlayerSelectedWeapon::Magic);

            // render magic wep info
            // magic icon sprite
            sf::Sprite invMagicInfoSprite(GameAssets::Get().damageTypesSpriteSheet, sf::IntRect(16, 0, 16, 16));
            invMagicInfoSprite.setPosition(62.0f, target.getView().getSize().y - 131.0f);

            target.draw(invMagicInfoSprite);

            // magic name
            sf::Text invMagicInfoNameText(std::string(), GameAssets::Get().gameFont, 8);
            invMagicInfoNameText.setPosition(82.0f, target.getView().getSize().y - 126.0f);
            invMagicInfoNameText.setColor(sf::Color(255, 255, 255));

            if (!playerInv->GetMagicWeapon() || playerInv->GetMagicWeapon()->GetAmount() <= 0) {
                invMagicInfoNameText.setString("None");
            }
            else {
                invMagicInfoNameText.setString(playerInv->GetMagicWeapon()->GetItemName());

                // magic stats
                std::ostringstream oss;
                oss << "Attack: " << playerInv->GetMagicWeapon()->GetAttack() << "\n";
                oss << "M Cost: " << playerInv->GetMagicWeapon()->GetManaCost() << "\n";
                oss << "Delay:  " << std::fixed << std::setprecision(1) <<
                    playerInv->GetMagicWeapon()->GetUseDelay().asSeconds() << "s";

                sf::Text invMagicInfoStatsText(oss.str(), GameAssets::Get().gameFont, 8);
                invMagicInfoStatsText.setPosition(64.0f, target.getView().getSize().y - 113.0f);
                invMagicInfoStatsText.setColor(sf::Color(255, 255, 255));

                Helper::RenderTextWithDropShadow(target, invMagicInfoStatsText);

                // magic desc
                sf::Text invMagicInfoDescText(playerInv->GetMagicWeapon()->GetShortDescription(),
                    GameAssets::Get().gameFont, 8);
                invMagicInfoDescText.setPosition(64.0f, target.getView().getSize().y - 83.0f);
                invMagicInfoDescText.setColor(sf::Color(255, 255, 0));

                Helper::RenderTextWithDropShadow(target, invMagicInfoDescText);
            }

            Helper::RenderTextWithDropShadow(target, invMagicInfoNameText);

            // render armour inv text label
            sf::Text invArmourLabelText("Armour", GameAssets::Get().gameFont, 8);
            invArmourLabelText.setPosition(5.0f,
                target.getView().getSize().y - 60.0f - invArmourLabelText.getGlobalBounds().height);
            invArmourLabelText.setColor(sf::Color(255, 255, 255));

            Helper::RenderTextWithDropShadow(target, invArmourLabelText);

            // render armour item
            RenderUIItem(target, sf::Vector2f(5.0f, target.getView().getSize().y - 55.0f),
                std::string(), playerInv->GetArmour());

            // render armour info
            // def icon sprite
            sf::Sprite invArmourInfoSprite(GameAssets::Get().damageTypesSpriteSheet, sf::IntRect(16, 16, 16, 16));
            invArmourInfoSprite.setPosition(62.0f, target.getView().getSize().y - 59.0f);

            target.draw(invArmourInfoSprite);

            // armour name
            sf::Text invArmourInfoNameText(std::string(), GameAssets::Get().gameFont, 8);
            invArmourInfoNameText.setPosition(82.0f, target.getView().getSize().y - 55.0f);
            invArmourInfoNameText.setColor(sf::Color(255, 255, 255));

            if (!playerInv->GetArmour() || playerInv->GetArmour()->GetAmount() <= 0) {
                invArmourInfoNameText.setString("None");
            }
            else {
                invArmourInfoNameText.setString(playerInv->GetArmour()->GetItemName());

                // armour stats
                std::ostringstream oss;
                oss << "Defence against:" << "\n";
                oss << "Melee : " << playerInv->GetArmour()->GetMeleeDefense() << "\n";
                oss << "Magic : " << playerInv->GetArmour()->GetMagicDefense() << "\n";

                sf::Text invArmourInfoStatsText(oss.str(), GameAssets::Get().gameFont, 8);
                invArmourInfoStatsText.setPosition(64.0f, target.getView().getSize().y - 42.0f);
                invArmourInfoStatsText.setColor(sf::Color(255, 255, 255));

                Helper::RenderTextWithDropShadow(target, invArmourInfoStatsText);

                // armour desc
                sf::Text invArmourInfoDescText(playerInv->GetArmour()->GetShortDescription(),
                    GameAssets::Get().gameFont, 8);
                invArmourInfoDescText.setPosition(64.0f, target.getView().getSize().y - 12.0f);
                invArmourInfoDescText.setColor(sf::Color(255, 255, 0));

                Helper::RenderTextWithDropShadow(target, invArmourInfoDescText);
            }

            target.draw(invArmourInfoNameText);

            // render potions inv text label
            sf::Text invPotionsLabelText("Potions", GameAssets::Get().gameFont, 8);
            invPotionsLabelText.setPosition(282.5f,
                target.getView().getSize().y - 60.0f - invPotionsLabelText.getGlobalBounds().height);
            invPotionsLabelText.setColor(sf::Color(255, 255, 255));

            Helper::RenderTextWithDropShadow(target, invPotionsLabelText);

            // render health potion item
            RenderUIItem(target, sf::Vector2f(282.5f, target.getView().getSize().y - 55.0f),
                "3", playerInv->GetHealthPotions());

            // render magic potion item
            RenderUIItem(target, sf::Vector2f(340.0f, target.getView().getSize().y - 55.0f),
                "4", playerInv->GetMagicPotions());
        }
    }
}


void Game::RenderUIMessages(sf::RenderTarget& target)
{
    for (auto it = messages_.rbegin(); it != messages_.rend(); ++it) {
        auto msg = *it;
        auto i = it - messages_.rbegin();

        auto alpha = static_cast<sf::Uint8>(255 * (1.0f - (static_cast<float>(i) / MaxMessages)));

        sf::Text msgText(msg.message, GameAssets::Get().gameFont, 8);
        msgText.setColor(sf::Color(msg.color.r, msg.color.g, msg.color.b, alpha));
        msgText.setPosition(5.0f, target.getView().getSize().y - 250.0f - 15.0f * i);

        Helper::RenderTextWithDropShadow(target, msgText, sf::Vector2f(2.0f, 2.0f),
            sf::Color(0, 0, 0, alpha));
    }
}


void Game::RenderUIDisplayedQuestion(sf::RenderTarget& target)
{
    if (!displayedQuestion_) {
        return;
    }

    // render window bg
    sf::RectangleShape uiBg(sf::Vector2f(765.0f, 250.0f));
    uiBg.setFillColor(sf::Color(20, 20, 20, 250));
    uiBg.setOutlineColor(sf::Color(0, 0, 0));
    uiBg.setOutlineThickness(-2.0f);
    uiBg.setPosition(0.5f * (target.getView().getSize() - uiBg.getSize()));

    target.draw(uiBg);

    // render window label
    sf::Text uiLabel("Answer this question correctly to unlock the chest:", GameAssets::Get().gameFont, 8);
    uiLabel.setPosition(uiBg.getPosition() + sf::Vector2f(0.5f * (uiBg.getSize().x - uiLabel.getGlobalBounds().width),
        20.0f));
    uiLabel.setColor(sf::Color(255, 255, 255));

    Helper::RenderTextWithDropShadow(target, uiLabel);

    // render question text
    sf::Text uiQuestion(displayedQuestion_->GetQuestion(), GameAssets::Get().gameFont, 10);
    uiQuestion.setPosition(uiBg.getPosition() +
        sf::Vector2f(0.5f * (uiBg.getSize().x - uiQuestion.getGlobalBounds().width), 50.0f));
    uiQuestion.setColor(sf::Color(255, 145, 0));

    Helper::RenderTextWithDropShadow(target, uiQuestion);

    // render choice a
    sf::Text uiChoiceA("Press 1: " + displayedQuestion_->GetAnswerChoice(displayedQuestionShuffledChoices_[0]),
        GameAssets::Get().gameFont, 10);
    uiChoiceA.setPosition(uiBg.getPosition() +
        sf::Vector2f(0.5f * (uiBg.getSize().x - uiChoiceA.getGlobalBounds().width), 100.0f));
    uiChoiceA.setColor(displayedQuestionSelectedChoice_ == 0 ? sf::Color(255, 255, 0) : sf::Color(255, 255, 255));
    uiChoiceA.setStyle(displayedQuestionSelectedChoice_ == 0 ? sf::Text::Underlined : sf::Text::Regular);

    Helper::RenderTextWithDropShadow(target, uiChoiceA);

    // render choice b
    sf::Text uiChoiceB("Press 2: " + displayedQuestion_->GetAnswerChoice(displayedQuestionShuffledChoices_[1]),
        GameAssets::Get().gameFont, 10);
    uiChoiceB.setPosition(uiBg.getPosition() +
        sf::Vector2f(0.5f * (uiBg.getSize().x - uiChoiceB.getGlobalBounds().width), 130.0f));
    uiChoiceB.setColor(displayedQuestionSelectedChoice_ == 1 ? sf::Color(255, 255, 0) : sf::Color(255, 255, 255));
    uiChoiceB.setStyle(displayedQuestionSelectedChoice_ == 1 ? sf::Text::Underlined : sf::Text::Regular);

    Helper::RenderTextWithDropShadow(target, uiChoiceB);

    // render choice c
    sf::Text uiChoiceC("Press 3: " + displayedQuestion_->GetAnswerChoice(displayedQuestionShuffledChoices_[2]),
        GameAssets::Get().gameFont, 10);
    uiChoiceC.setPosition(uiBg.getPosition() +
        sf::Vector2f(0.5f * (uiBg.getSize().x - uiChoiceC.getGlobalBounds().width), 160.0f));
    uiChoiceC.setColor(displayedQuestionSelectedChoice_ == 2 ? sf::Color(255, 255, 0) : sf::Color(255, 255, 255));
    uiChoiceC.setStyle(displayedQuestionSelectedChoice_ == 2 ? sf::Text::Underlined : sf::Text::Regular);

    Helper::RenderTextWithDropShadow(target, uiChoiceC);

    // render confirm label
    sf::Text uiConfirmLabel("Press ENTER to confirm your answer.", GameAssets::Get().gameFont, 8);
    uiConfirmLabel.setPosition(uiBg.getPosition() +
        sf::Vector2f(0.5f * (uiBg.getSize().x - uiConfirmLabel.getGlobalBounds().width), 220.0f));
    uiConfirmLabel.setColor(sf::Color(255, 255, 0));

    Helper::RenderTextWithDropShadow(target, uiConfirmLabel);
}


void Game::RenderUIRespawnSacrifice(sf::RenderTarget& target)
{
    auto player = GetPlayerEntity();

    // don't render if the player is alive...
    if (!player || !player->GetStats() || player->GetStats()->IsAlive()) {
        return;
    }

    // render window bg
    sf::RectangleShape uiBg(sf::Vector2f(600.0f, 250.0f));
    uiBg.setFillColor(sf::Color(20, 20, 20, 250));
    uiBg.setOutlineColor(sf::Color(0, 0, 0));
    uiBg.setOutlineThickness(-2.0f);
    uiBg.setPosition(0.5f * (target.getView().getSize() - uiBg.getSize()));

    target.draw(uiBg);

    // render window label
    sf::Text uiLabel("Oh dear - Bob has been knocked out!", GameAssets::Get().gameFont, 16);
    uiLabel.setPosition(uiBg.getPosition() + sf::Vector2f(0.5f * (uiBg.getSize().x - uiLabel.getGlobalBounds().width),
        20.0f));
    uiLabel.setColor(sf::Color(255, 50, 50));

    Helper::RenderTextWithDropShadow(target, uiLabel);

    // render sacrifice labels
    sf::Text uiSacrificeLabel1("However, not all hope is lost!", GameAssets::Get().gameFont, 10);
    uiSacrificeLabel1.setPosition(uiBg.getPosition() +
        sf::Vector2f(0.5f * (uiBg.getSize().x - uiSacrificeLabel1.getGlobalBounds().width), 60.0f));
    uiSacrificeLabel1.setColor(sf::Color(255, 255, 255));

    Helper::RenderTextWithDropShadow(target, uiSacrificeLabel1);

    sf::Text uiSacrificeLabel2("Bob can be revived, but at a price...", GameAssets::Get().gameFont, 10);
    uiSacrificeLabel2.setPosition(uiBg.getPosition() +
        sf::Vector2f(0.5f * (uiBg.getSize().x - uiSacrificeLabel2.getGlobalBounds().width), 80.0f));
    uiSacrificeLabel2.setColor(sf::Color(255, 255, 255));

    Helper::RenderTextWithDropShadow(target, uiSacrificeLabel2);

    sf::Text uiSacrificeLabel3("You will lose the following things after you have been revived:", GameAssets::Get().gameFont, 8);
    uiSacrificeLabel3.setPosition(uiBg.getPosition() +
        sf::Vector2f(0.5f * (uiBg.getSize().x - uiSacrificeLabel3.getGlobalBounds().width), 110.0f));
    uiSacrificeLabel3.setColor(sf::Color(255, 145, 0));

    Helper::RenderTextWithDropShadow(target, uiSacrificeLabel3);

    // render sacrifice items labels
    auto pInv = player->GetInventory();

    float sacrificeListItemY = 130.0f;

    // render list labels
    for (int i = 0; i < 4; ++i) {
        sf::Text uiSacrificeItemLabel(std::string(), GameAssets::Get().gameFont, 8);
        uiSacrificeItemLabel.setColor(sf::Color(255, 255, 255));

        if (i == 0) {
            // lose mana
            uiSacrificeItemLabel.setString("Any remaining Mana that you have");
            uiSacrificeItemLabel.setColor(sf::Color(100, 100, 255));
        }
        else if (pInv) {
            // lose items

            if (i == 1) {
                // lose magic item
                if (!pInv->GetMagicWeapon() || pInv->GetMagicWeapon()->GetAmount() <= 0) {
                    continue;
                }

                uiSacrificeItemLabel.setString("Your " + pInv->GetMagicWeapon()->GetItemName());
            }
            else if (i == 2) {
                // lose health potions
                if (!pInv->GetHealthPotions() || pInv->GetHealthPotions()->GetAmount() <= 0) {
                    continue;
                }

                uiSacrificeItemLabel.setString("All of your Health Potions");
                uiSacrificeItemLabel.setColor(sf::Color(255, 100, 100));
            }
            else if (i == 3) {
                // lose magic potions
                if (!pInv->GetMagicPotions() || pInv->GetMagicPotions()->GetAmount() <= 0) {
                    continue;
                }

                uiSacrificeItemLabel.setString("All of your Magic Potions");
                uiSacrificeItemLabel.setColor(sf::Color(100, 100, 255));
            }
        }

        uiSacrificeItemLabel.setPosition(uiBg.getPosition() +
            sf::Vector2f(0.5f * (uiBg.getSize().x - uiSacrificeItemLabel.getGlobalBounds().width),
            sacrificeListItemY));

        Helper::RenderTextWithDropShadow(target, uiSacrificeItemLabel);

        sacrificeListItemY += 12.0f;
    }

    // render confirm label
    sf::Text uiConfirmLabel("Press ENTER to be revived.", GameAssets::Get().gameFont, 8);
    uiConfirmLabel.setPosition(uiBg.getPosition() +
        sf::Vector2f(0.5f * (uiBg.getSize().x - uiConfirmLabel.getGlobalBounds().width), 220.0f));
    uiConfirmLabel.setColor(sf::Color(255, 255, 0));

    Helper::RenderTextWithDropShadow(target, uiConfirmLabel);
}


void Game::RenderUILowStatsWarning(sf::RenderTarget& target)
{
    auto player = GetPlayerEntity();

    if (!player || !player->GetStats() || !player->GetStats()->IsAlive()) {
        return;
    }

    if (IsPlayerLowHealth()) {
        sf::Text uiLowHealth("Low Health!", GameAssets::Get().gameFont, 12);
        uiLowHealth.setColor(sf::Color(255, 0, 0));
        uiLowHealth.setPosition(0.5f * (target.getView().getSize().x - uiLowHealth.getGlobalBounds().width),
            0.8f * target.getView().getSize().y);

        Helper::RenderTextWithDropShadow(target, uiLowHealth);
    }

    // TODO: rely on weapon mana cost instead?
    //if (IsPlayerLowMana()) {
    //    sf::Text uiLowMana("Low Mana!", GameAssets::Get().gameFont, 12);
    //    uiLowMana.setColor(sf::Color(0, 0, 255));
    //    uiLowMana.setPosition(0.5f * (target.getView().getSize().x - uiLowMana.getGlobalBounds().width),
    //        0.8f * target.getView().getSize().y + 18.0f);

    //    Helper::RenderTextWithDropShadow(target, uiLowMana);
    //}
}


void Game::RenderUIMapMode(sf::RenderTarget& target)
{
    sf::Text mapLabel("You are in Map Mode.", GameAssets::Get().gameFont, 16);
    mapLabel.setColor(sf::Color(255, 255, 255));
    mapLabel.setPosition(5.0f, target.getView().getSize().y - 40.0f);

    Helper::RenderTextWithDropShadow(target, mapLabel);

    sf::Text mapControls("Press M or TAB to disable.", GameAssets::Get().gameFont, 10);
    mapControls.setColor(sf::Color(255, 255, 0));
    mapControls.setPosition(5.0f, target.getView().getSize().y - 20.0f);

    Helper::RenderTextWithDropShadow(target, mapControls);
}


void Game::RenderUIMenu(sf::RenderTarget& target)
{
    sf::RectangleShape menuBack(target.getView().getSize());
    menuBack.setPosition(sf::Vector2f());
    menuBack.setFillColor(sf::Color(30, 30, 30, 200));

    target.draw(menuBack);

    sf::Text menuTitleThe("The", GameAssets::Get().gameFont, 18);
    menuTitleThe.setColor(sf::Color(200, 200, 200));
    menuTitleThe.setPosition(0.5f * (target.getView().getSize().x - menuTitleThe.getGlobalBounds().width),
        50.0f);

    Helper::RenderTextWithDropShadow(target, menuTitleThe, sf::Vector2f(5.0f, 5.0f));

    sf::Text menuTitleFs("File System Dungeon", GameAssets::Get().gameFont, 32);
    menuTitleFs.setColor(sf::Color(255, 255, 255));
    menuTitleFs.setPosition(0.5f * (target.getView().getSize().x - menuTitleFs.getGlobalBounds().width),
        80.0f);

    Helper::RenderTextWithDropShadow(target, menuTitleFs, sf::Vector2f(5.0f, 5.0f));

    sf::Text menuCreds("By Sean Dewar", GameAssets::Get().altFont, 10);
    menuCreds.setColor(sf::Color(200, 200, 200));
    menuCreds.setPosition(0.5f * (target.getView().getSize().x - menuCreds.getGlobalBounds().width),
        125.0f);

    Helper::RenderTextWithDropShadow(target, menuCreds);

    sf::Text menuCont("Press ENTER to play!", GameAssets::Get().gameFont, 12);
    menuCont.setColor(sf::Color(255, 255, 0));
    menuCont.setPosition(0.5f * (target.getView().getSize().x - menuCont.getGlobalBounds().width),
        155.0f);

    Helper::RenderTextWithDropShadow(target, menuCont);

    sf::Sprite menuBob(GameAssets::Get().playerSpriteSheet, sf::IntRect(0, 0, 16, 16));
    menuBob.setScale(5.0f, 5.0f);
    menuBob.setPosition(0.5f * (target.getView().getSize().x - menuBob.getGlobalBounds().width), 175.0f);

    target.draw(menuBob);

    std::ostringstream oss;
    oss << "This is Bob; an archaeologist who specializes in\n";
    oss << "exploring lost ruins to find all sorts of valuable\n";
    oss << "artefacts and other treasures.\n\n";

    oss << "In his most recent archaeological venture however,\n";
    oss << "Bob managed to get himself trapped within a large\n";
    oss << "underground dungeon containing a disorienting amount\n";
    oss << "of winding passages and staircases that seemingly\n";
    oss << "led to everywhere other than the exit he wished to find.\n\n";

    oss << "What Bob doesn't know however, is that this dungeon is\n";
    oss << "very special, as its structure reflects the layout of\n";
    oss << "files and directories within a UNIX-style file system.\n";
    oss << "This is unfortunate for Bob, as he knows nothing about\n";
    oss << "computers whatsoever!";

    sf::Text menuDesc1(oss.str(), GameAssets::Get().altFont, 12);
    menuDesc1.setColor(sf::Color(200, 200, 200));
    menuDesc1.setPosition(0.5f * (target.getView().getSize().x - menuDesc1.getGlobalBounds().width),
        275.0f);

    Helper::RenderTextWithDropShadow(target, menuDesc1);

    sf::Sprite menuChest(GameAssets::Get().chestsSpriteSheet, sf::IntRect(16, 0, 16, 16));
    menuChest.setScale(5.0f, 5.0f);
    menuChest.setPosition(0.5f * (target.getView().getSize().x - menuChest.getGlobalBounds().width),
        465.0f);

    target.draw(menuChest);

    sf::Sprite menuArt1(GameAssets::Get().itemsSpriteSheet, sf::IntRect(0, 32, 16, 16));
    menuArt1.setScale(4.0f, 4.0f);
    menuArt1.setPosition(0.5f * (target.getView().getSize().x - 130.0f), 495.0f);

    target.draw(menuArt1);

    sf::Sprite menuArt2(GameAssets::Get().itemsSpriteSheet, sf::IntRect(0, 64, 16, 16));
    menuArt2.setScale(4.0f, 4.0f);
    menuArt2.setPosition(0.5f * (target.getView().getSize().x + 30.0f), 490.0f);

    target.draw(menuArt2);

    sf::Sprite menuStaff1(GameAssets::Get().itemsSpriteSheet, sf::IntRect(32, 16, 16, 16));
    menuStaff1.setScale(4.0f, 4.0f);
    menuStaff1.setPosition(0.5f * (target.getView().getSize().x + 100.0f), 480.0f);

    target.draw(menuStaff1);

    sf::Sprite menuSword1(GameAssets::Get().itemsSpriteSheet, sf::IntRect(32, 0, 16, 16));
    menuSword1.setScale(4.0f, 4.0f);
    menuSword1.setPosition(0.5f * (target.getView().getSize().x + 180.0f), 480.0f);

    target.draw(menuSword1);

    sf::Sprite menuArm1(GameAssets::Get().itemsSpriteSheet, sf::IntRect(48, 32, 16, 16));
    menuArm1.setScale(4.0f, 4.0f);
    menuArm1.setPosition(0.5f * (target.getView().getSize().x - 64.5f), 505.0f);

    target.draw(menuArm1);

    sf::Sprite menuHPot(GameAssets::Get().itemsSpriteSheet, sf::IntRect(0, 0, 16, 16));
    menuHPot.setScale(4.0f, 4.0f);
    menuHPot.setPosition(0.5f * (target.getView().getSize().x - 220.0f), 480.0f);

    target.draw(menuHPot);

    sf::Sprite menuMPot(GameAssets::Get().itemsSpriteSheet, sf::IntRect(0, 16, 16, 16));
    menuMPot.setScale(4.0f, 4.0f);
    menuMPot.setPosition(0.5f * (target.getView().getSize().x - 300.0f), 480.0f);

    target.draw(menuMPot);

    sf::Sprite menuBad1(GameAssets::Get().enemySpriteSheet, sf::IntRect(16, 0, 16, 16));
    menuBad1.setScale(5.0f, 5.0f);
    menuBad1.setPosition(0.5f * (target.getView().getSize().x - 500.0f), 475.0f);

    target.draw(menuBad1);

    sf::Sprite menuBad2(GameAssets::Get().enemySpriteSheet, sf::IntRect(0, 80, 16, 16));
    menuBad2.setScale(5.0f, 5.0f);
    menuBad2.setPosition(0.5f * (target.getView().getSize().x + 350.0f), 475.0f);

    target.draw(menuBad2);

    oss = std::ostringstream();
    oss << "Here's where you come in: your job is to help Bob\n";
    oss << "escape by locating 9 magical artefact pieces from within\n";
    oss << "the dungeon while protecting Bob from the baddies inside!\n\n";

    oss << "Be warned however, as you will need to know how to\n";
    oss << "interpret UNIX path names in order to locate the artefact\n";
    oss << "pieces while also being able to correctly answer some\n";
    oss << "questions relating to the structure of UNIX-style file\n";
    oss << "systems and their properties!\n\n";
    
    oss << "This is nothing that a bit of research can't solve -\n";
    oss << "Good luck!";

    sf::Text menuDesc2(oss.str(), GameAssets::Get().altFont, 12);
    menuDesc2.setColor(sf::Color(200, 200, 200));
    menuDesc2.setPosition(0.5f * (target.getView().getSize().x - menuDesc2.getGlobalBounds().width),
        570.0f);

    Helper::RenderTextWithDropShadow(target, menuDesc2);
}


void Game::Render(sf::RenderTarget& target)
{
    target.clear();

    if (world_) {
        UpdateCamera(target);

        // debug mode
        if (debugMode_) {
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::LControl)) {
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
        }

        world_->Render(target);

        // state specific ui
        if (state_ == GameState::InGame) {
            RenderUILocation(target);
            RenderUIObjective(target);
            RenderUIPlayerStats(target);

            // hide some ui if in map mode to reduce screen clutter
            if (mapMode_) {
                RenderUIMapMode(target);
            }
            else {
                RenderUIPlayerInventory(target);
                RenderUIMessages(target);
                RenderUIControls(target);
            }

            auto player = GetPlayerEntity();

            RenderUILowStatsWarning(target);

            if (player && player->GetStats() && !player->GetStats()->IsAlive()) {
                RenderUIRespawnSacrifice(target);
            }
            else if (displayedQuestion_) {
                RenderUIDisplayedQuestion(target);
            }
            else {
                RenderUIPlayerUseTargetText(target);
            }
        }
        else if (state_ == GameState::Menu1) {
            RenderUIMenu(target);
        }

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