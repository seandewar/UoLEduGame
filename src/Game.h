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
    bool debugMode_;

    std::vector<sf::Keyboard::Key> eventKeysPressed_;

    std::unique_ptr<GameFilesystem> worldFs_;
	std::unique_ptr<World> world_;

    EntityId playerId_;

    std::string scheduledLevelChangeFsNodePath_;

    inline WorldArea* GetWorldArea() { return (world_ ? world_->GetCurrentArea() : nullptr); }

    inline PlayerEntity* GetPlayerEntity()
    { 
        auto area = GetWorldArea();
        return area ? area->GetEntity<PlayerEntity>(playerId_) : nullptr;
    }
    
    bool SpawnPlayer(const sf::Vector2f& pos);
    bool RemovePlayer();

    bool ChangeLevel(const std::string& fsNodePath);

    void ViewFollowPlayer();

    void RenderUILocation(sf::RenderTarget& target);
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

    void RunFrame(sf::RenderTarget& target);

    bool NewGame();
    inline void ScheduleLevelChange(const std::string& fsNodePath) { scheduledLevelChangeFsNodePath_ = fsNodePath; }

    inline void SetDebugMode(bool debugMode) { debugMode_ = debugMode; }
    inline bool IsInDebugMode() const { return debugMode_; }
};

