#pragma once

#include <memory>

#include <SFML/Graphics/RenderTarget.hpp>
#include <SFML/Graphics/Texture.hpp>
#include <SFML/Graphics/Font.hpp>
#include <SFML/System/Time.hpp>

#include "GameFilesystem.h"
#include "World.h"

/**
* Struct containing loaded assets
*/
struct GameAssets
{
    static GameAssets& Get()
    {
        static GameAssets instance;
        return instance;
    }

    sf::Font gameFont;

    sf::Texture viewVignette;
    sf::Texture genericTilesSheet;

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

	sf::RenderTarget& target_;

    std::unique_ptr<GameFilesystem> worldFs_;
	std::unique_ptr<World> world_;

    EntityId playerId_;

    inline WorldArea* GetWorldArea() { return (world_ ? world_->GetCurrentArea() : nullptr); }
    
    bool SpawnPlayer(const sf::Vector2f& pos);
    void ViewFollowPlayer();

    void RenderVignette();
    void RenderUILocation();

public:
    static const sf::Time FrameTimeStep;

	Game(sf::RenderTarget& target);
	~Game();

    bool Init();

    bool NewGame();

    void Tick();
    void Render();

    inline void SetDebugMode(bool debugMode) { debugMode_ = debugMode; }
    inline bool IsInDebugMode() const { return debugMode_; }
};

