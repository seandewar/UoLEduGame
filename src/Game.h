#pragma once

#include <memory>

#include <SFML/Graphics/RenderTarget.hpp>
#include <SFML/Graphics/Texture.hpp>
#include <SFML/Graphics/Font.hpp>
#include <SFML/System/Time.hpp>
#include <SFML/Window/Keyboard.hpp>
#include <SFML/Audio/SoundBuffer.hpp>
#include <SFML/Audio/Sound.hpp>

#include "GameFilesystem.h"
#include "GameDirector.h"
#include "World.h"
#include "Player.h"

/**
* Struct containing loaded assets
*/
struct GameAssets
{
    static inline GameAssets& Get()
    {
        static GameAssets instance;
        return instance;
    }

    sf::Font gameFont;
    sf::Font altFont;

    sf::Texture sfmlLogo;
    sf::Texture uolLogo;

    sf::Texture viewVignette;
    sf::Texture genericTilesSheet;
    sf::Texture stairsSpriteSheet;
    sf::Texture playerSpriteSheet;
    sf::Texture chestsSpriteSheet;
    sf::Texture itemsSpriteSheet;
    sf::Texture altarSpriteSheet;
    sf::Texture enemySpriteSheet;
    sf::Texture damageTypesSpriteSheet;
    sf::Texture effectSpriteSheet;
    sf::Texture sparkleSpriteSheet;
    sf::Texture projectileSpriteSheet;

    sf::Sound selectSound;
    sf::Sound drinkSound;
    sf::Sound blastSound;
    sf::Sound waveSound;
    sf::Sound drainSound;
    sf::Sound zeroBlastSound;
    sf::Sound pickupSound;
    sf::Sound artefactPickupSound;
    sf::Sound attackSound;
    sf::Sound hitSound;
    sf::Sound specSound;
    sf::Sound lowHealthSound;
    sf::Sound deathSound;
    sf::Sound playerHurtSound;
    sf::Sound playerDeathSound;
    sf::Sound successSound;
    sf::Sound failureSound;
    sf::Sound openChestSound;
    sf::Sound blockSound;
    sf::Sound armourPenSound;
    sf::Sound invincibilitySound;
    sf::Sound magicFireSound;
    sf::Sound smokeSound;
    sf::Sound bossSwordSound;
    sf::Sound bossSpawnSound;
    sf::Sound bossDyingSound;
    sf::Sound bossDeadSound;
    sf::Sound bossActionSound;

    bool LoadAssets();

private:
    sf::SoundBuffer drinkSoundBuffer;
    sf::SoundBuffer blastSoundBuffer;
    sf::SoundBuffer waveSoundBuffer;
    sf::SoundBuffer drainSoundBuffer;
    sf::SoundBuffer zeroBlastSoundBuffer;
    sf::SoundBuffer hitSoundBuffer;
    sf::SoundBuffer specSoundBuffer;
    sf::SoundBuffer lowHealthSoundBuffer;
    sf::SoundBuffer deathSoundBuffer;
    sf::SoundBuffer playerHurtSoundBuffer;
    sf::SoundBuffer playerDeathSoundBuffer;
    sf::SoundBuffer pickupSoundBuffer;
    sf::SoundBuffer pickup2SoundBuffer;
    sf::SoundBuffer selectSoundBuffer;
    sf::SoundBuffer attackSoundBuffer;
    sf::SoundBuffer successSoundBuffer;
    sf::SoundBuffer failureSoundBuffer;
    sf::SoundBuffer openChestSoundBuffer;
    sf::SoundBuffer blockSoundBuffer;
    sf::SoundBuffer armourPenSoundBuffer;
    sf::SoundBuffer invincibilitySoundBuffer;
    sf::SoundBuffer magicFireSoundBuffer;
    sf::SoundBuffer smokeSoundBuffer;
    sf::SoundBuffer bossSpawnSoundBuffer;
    sf::SoundBuffer bossSwordSoundBuffer;
    sf::SoundBuffer bossDyingSoundBuffer;
    sf::SoundBuffer bossDeadSoundBuffer;
    sf::SoundBuffer bossActionSoundBuffer;

    GameAssets() { }
    ~GameAssets() { }
};

/**
* List of game states
*/
enum class GameState
{
    Menu1,
    MenuCredits,
    InGame
};

/**
* Main Game class
*/
class Game
{
    struct GameMessage {
        sf::Color color;
        std::string message;

        GameMessage(const std::string& message, const sf::Color& color) :
            color(color),
            message(message)
        { }

        ~GameMessage() { }
    };

    static const std::size_t MaxMessages = 10;

    bool debugMode_;

    GameState state_;

    std::vector<sf::Keyboard::Key> eventKeysPressed_;

    std::vector<GameMessage> messages_;

    std::unique_ptr<GameFilesystem> worldFs_;
	std::unique_ptr<World> world_;

    bool mapMode_;

    GameDirector director_;

    AliveStats playerStats_;
    PlayerInventory playerInv_;
    sf::Time playerCachedInvincTimeLeft_;
    EntityId playerId_;

    std::string scheduledLevelChangeFsNodePath_;
    bool scheduledNewGame_;

    const IGameQuestion* displayedQuestion_;
    std::vector<GameQuestionAnswerChoice> displayedQuestionShuffledChoices_;
    int displayedQuestionSelectedChoice_;

    sf::Time lowHealthNextBeepTimeLeft_;

    int endPlayerNumDeaths_;
    u32 endPlayerNumDamageTaken_;
    u32 endPlayerNumDamageGiven_;
    u32 endPlayerNumQuestionsWrong_;
    sf::Time endPlayerTimeTaken_;

    inline WorldArea* GetWorldArea() { return world_ ? world_->GetCurrentArea() : nullptr; }
    inline const WorldArea* GetWorldArea() const { return world_ ? world_->GetCurrentArea() : nullptr; }

    inline PlayerEntity* GetPlayerEntity()
    {
        auto area = GetWorldArea();
        return area ? area->GetEntity<PlayerEntity>(playerId_) : nullptr;
    }
    inline const PlayerEntity* GetPlayerEntity() const
    {
        auto area = GetWorldArea();
        return area ? area->GetEntity<PlayerEntity>(playerId_) : nullptr;
    }

    inline bool IsPlayerLowHealth() const
    {
        auto player = GetPlayerEntity();

        return player && player->GetStats() &&
            (player->GetStats()->GetHealth() <= player->GetStats()->GetMaxHealth() / 3);
    }

    inline bool IsPlayerLowMana() const
    {
        auto player = GetPlayerEntity();

        return player && player->GetStats() &&
            (player->GetStats()->GetMana() <= player->GetStats()->GetMaxMana() / 3);
    }

    void ResetPlayerStats();

    bool SpawnPlayer(const sf::Vector2f* optionalStartPos = nullptr);
    bool RemovePlayer();
    bool TeleportPlayerToObjective();

    bool ChangeLevel(const std::string& fsNodePath);
    bool NewGame();

    void UpdateCamera(sf::RenderTarget& target);

    void HandleDisplayedQuestionInput();
    void HandleRespawnSacrificeInput();
    void HandleUseInventory();
    void HandlePlayerMoveInput();
    void HandlePlayerLowHealthBeep();

    void RenderUIMessages(sf::RenderTarget& target);
    void RenderUILoadingNewGame(sf::RenderTarget& target);
    void RenderUILoadingArea(sf::RenderTarget& target);
    void RenderUILocation(sf::RenderTarget& target);
    void RenderUIObjective(sf::RenderTarget& target);
    void RenderUIPlayerStats(sf::RenderTarget& target);
    void RenderUIItem(sf::RenderTarget& target, const sf::Vector2f& position, const std::string& label,
        Item* item, bool isHighlighted = false);
    void RenderUILowStatsWarning(sf::RenderTarget& target);
    void RenderUIPlayerInventory(sf::RenderTarget& target);
    void RenderUIDisplayedQuestion(sf::RenderTarget& target);
    void RenderUIRespawnSacrifice(sf::RenderTarget& target);
    void RenderUIPlayerUseTargetText(sf::RenderTarget& target);
    void RenderUIControls(sf::RenderTarget& target);
    void RenderUIMapMode(sf::RenderTarget& target);
    void RenderUIEndStats(sf::RenderTarget& target);

    void RenderUIMenu(sf::RenderTarget& target);

    void Tick();
    void Render(sf::RenderTarget& target);

    Game();
    ~Game();

public:
    static const sf::Time FrameTimeStep;

    static inline Game& Get()
    {
        static Game instance;
        return instance;
    }

    bool Init();

    inline void AddPressedEventKey(sf::Keyboard::Key key) { eventKeysPressed_.emplace_back(key); }

    /**
    * Returns true if a key is pressed from a window event
    * (not using real-time input), false otherwise.
    */
    inline bool IsKeyPressedFromEvent(sf::Keyboard::Key key)
    {
        return std::find(eventKeysPressed_.begin(), eventKeysPressed_.end(), key) != eventKeysPressed_.end();
    }

    inline void AddMessage(const std::string& message, const sf::Color& color = sf::Color(255, 255, 255))
    {
        if (messages_.size() >= MaxMessages) {
            messages_.erase(messages_.begin());
        }

        messages_.emplace_back(message, color);
    }

    inline void NotifyPlayerDeath()
    {
        if (director_.GetCurrentObjectiveType() != GameObjectiveType::End &&
            director_.GetCurrentObjectiveType() != GameObjectiveType::NotStarted) {
            ++endPlayerNumDeaths_;
        }
    }

    inline void NotifyPlayerDamaged(u32 damageAmount)
    {
        if (director_.GetCurrentObjectiveType() != GameObjectiveType::End &&
            director_.GetCurrentObjectiveType() != GameObjectiveType::NotStarted) {
            endPlayerNumDamageTaken_ += damageAmount;
        }
    }

    inline void NotifyPlayerDamageGiven(u32 damageGiven)
    {
        if (director_.GetCurrentObjectiveType() != GameObjectiveType::End &&
            director_.GetCurrentObjectiveType() != GameObjectiveType::NotStarted) {
            endPlayerNumDamageGiven_ += damageGiven;
        }
    }

    inline const IGameQuestion* GetDisplayedQuestion() const { return displayedQuestion_; }
    void ResetDisplayedQuestion();
    void SetDisplayedQuestion(const IGameQuestion* question);

    void RunFrame(sf::RenderTarget& target);

    inline void SetLevelChange(const std::string& fsNodePath) { scheduledLevelChangeFsNodePath_ = fsNodePath; }
    inline void ScheduleNewGame() { scheduledNewGame_ = true; }

    inline GameDirector& GetDirector() { return director_; }

    inline bool IsInMapMode() const { return mapMode_; }

    inline void SetDebugMode(bool debugMode) { debugMode_ = debugMode; }
    inline bool IsInDebugMode() const { return debugMode_; }

    inline GameState GetCurrentGameState() const { return state_; }
};

