#pragma once

#include <memory>

#include <SFML/Graphics/RenderTarget.hpp>
#include <SFML/Graphics/Texture.hpp>
#include <SFML/Graphics/Font.hpp>
#include <SFML/System/Time.hpp>
#include <SFML/Window/Keyboard.hpp>

#include "GameFilesystem.h"
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

    sf::Texture viewVignette;
    sf::Texture genericTilesSheet;
    sf::Texture stairsSpriteSheet;
    sf::Texture playerSpriteSheet;
    sf::Texture chestsSpriteSheet;
    sf::Texture itemsSpriteSheet;

    bool LoadAssets();

private:
    GameAssets() { }
    ~GameAssets() { }
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

    static const std::size_t MaxMessages = 8;

    bool debugMode_;

    std::vector<sf::Keyboard::Key> eventKeysPressed_;

    std::vector<GameMessage> messages_;

    std::unique_ptr<GameFilesystem> worldFs_;
	std::unique_ptr<World> world_;

    AliveStats playerStats_;
    PlayerInventory playerInv_;
    EntityId playerId_;

    std::string scheduledLevelChangeFsNodePath_;

    inline WorldArea* GetWorldArea() { return world_ ? world_->GetCurrentArea() : nullptr; }

    inline PlayerEntity* GetPlayerEntity()
    { 
        auto area = GetWorldArea();
        return area ? area->GetEntity<PlayerEntity>(playerId_) : nullptr;
    }

    void ResetPlayerStats();

    bool SpawnPlayer(const sf::Vector2f* optionalStartPos = nullptr);
    bool RemovePlayer();

    bool ChangeLevel(const std::string& fsNodePath);

    void ViewFollowPlayer();

    void HandleUseInventory();

    void RenderUIMessages(sf::RenderTarget& target);

    void RenderUILoadingArea(sf::RenderTarget& target);
    void RenderUILocation(sf::RenderTarget& target);
    void RenderUIPlayerStats(sf::RenderTarget& target);

    void RenderUIItem(sf::RenderTarget& target, const sf::Vector2f& position, const std::string& label,
        Item* item, bool isHighlighted = false);
    void RenderUIPlayerInventory(sf::RenderTarget& target);

    void RenderUIPlayerUseTargetText(sf::RenderTarget& target);

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

    void RunFrame(sf::RenderTarget& target);

    bool NewGame();
    inline void SetLevelChange(const std::string& fsNodePath) { scheduledLevelChangeFsNodePath_ = fsNodePath; }

    inline void SetDebugMode(bool debugMode) { debugMode_ = debugMode; }
    inline bool IsInDebugMode() const { return debugMode_; }
};

