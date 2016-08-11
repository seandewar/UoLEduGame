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

    sf::SoundBuffer drinkSoundBuffer;
    sf::SoundBuffer blastSoundBuffer;
    sf::SoundBuffer drainSoundBuffer;
    sf::SoundBuffer zeroBlastSoundBuffer;
    sf::SoundBuffer hitSoundBuffer;
    sf::SoundBuffer specSoundBuffer;
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

    sf::Sound selectSound;
    sf::Sound drinkSound;
    sf::Sound blastSound;
    sf::Sound drainSound;
    sf::Sound zeroBlastSound;
    sf::Sound pickupSound;
    sf::Sound artefactPickupSound;
    sf::Sound attackSound;
    sf::Sound hitSound;
    sf::Sound specSound;
    sf::Sound deathSound;
    sf::Sound playerHurtSound;
    sf::Sound playerDeathSound;
    sf::Sound successSound;
    sf::Sound failureSound;
    sf::Sound openChestSound;
    sf::Sound blockSound;
    sf::Sound armourPenSound;
    sf::Sound invincibilitySound;

    bool LoadAssets();

private:
    GameAssets() { }
    ~GameAssets() { }
};

/**
* List of game states
*/
enum class GameState
{
    Menu1,
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

    GameDirector director_;

    AliveStats playerStats_;
    PlayerInventory playerInv_;
    sf::Time playerCachedInvincTimeLeft_;
    EntityId playerId_;

    std::string scheduledLevelChangeFsNodePath_;

    const IGameQuestion* displayedQuestion_;
    std::vector<GameQuestionAnswerChoice> displayedQuestionShuffledChoices_;
    int displayedQuestionSelectedChoice_;

    inline WorldArea* GetWorldArea() { return world_ ? world_->GetCurrentArea() : nullptr; }

    inline PlayerEntity* GetPlayerEntity()
    {
        auto area = GetWorldArea();
        return area ? area->GetEntity<PlayerEntity>(playerId_) : nullptr;
    }

    void ResetPlayerStats();

    bool SpawnPlayer(const sf::Vector2f* optionalStartPos = nullptr);
    bool RemovePlayer();
    bool TeleportPlayerToObjective();

    bool ChangeLevel(const std::string& fsNodePath);

    void UpdateCamera(sf::RenderTarget& target);

    void HandleDisplayedQuestionInput();
    void HandleRespawnSacrificeInput();
    void HandleUseInventory();
    void HandlePlayerMoveInput();

    void RenderUIMessages(sf::RenderTarget& target);
    void RenderUILoadingArea(sf::RenderTarget& target);
    void RenderUILocation(sf::RenderTarget& target);
    void RenderUIObjective(sf::RenderTarget& target);
    void RenderUIPlayerStats(sf::RenderTarget& target);
    void RenderUIItem(sf::RenderTarget& target, const sf::Vector2f& position, const std::string& label,
        Item* item, bool isHighlighted = false);
    void RenderUIPlayerInventory(sf::RenderTarget& target);
    void RenderUIDisplayedQuestion(sf::RenderTarget& target);
    void RenderUIRespawnSacrifice(sf::RenderTarget& target);
    void RenderUIPlayerUseTargetText(sf::RenderTarget& target);
    void RenderUIControls(sf::RenderTarget& target);

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

    inline const IGameQuestion* GetDisplayedQuestion() const { return displayedQuestion_; }
    void ResetDisplayedQuestion();
    void SetDisplayedQuestion(const IGameQuestion* question);

    void RunFrame(sf::RenderTarget& target);

    bool NewGame();
    inline void SetLevelChange(const std::string& fsNodePath) { scheduledLevelChangeFsNodePath_ = fsNodePath; }

    inline GameDirector& GetDirector() { return director_; }

    inline void SetDebugMode(bool debugMode) { debugMode_ = debugMode; }
    inline bool IsInDebugMode() const { return debugMode_; }

    inline GameState GetCurrentGameState() const { return state_; }
};

