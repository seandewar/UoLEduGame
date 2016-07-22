#pragma once

#include <vector>

#include <SFML/System/Time.hpp>
#include <SFML/Graphics/Sprite.hpp>
#include <SFML/Graphics/RenderTarget.hpp>

/**
* Class for handling basic animation.
*/
class Animation
{
    struct AnimKeyFrame {
        sf::Time time;
        sf::Sprite sprite;

        AnimKeyFrame(const sf::Time& time, const sf::Sprite& sprite) :
            time(time),
            sprite(sprite)
        { }

        ~AnimKeyFrame() { }
    };

    std::vector<AnimKeyFrame> frames_;
    std::size_t currentFrame_;
    sf::Time currentFrameRemainingTime_;

public:
    Animation();
    ~Animation();

    void Tick();

    inline void SetCurrentFrame(std::size_t i) { currentFrame_ = std::max<std::size_t>(0, std::min(frames_.size(), i)); }

    inline sf::Sprite GetCurrentFrame() const { return frames_.size() > 0 ? GetFrame(currentFrame_) : sf::Sprite(); }
    inline std::size_t GetCurrentFrameIndex() const { return currentFrame_; }

    inline void Restart() { currentFrame_ = 0; }

    inline void AddFrame(const sf::Sprite& sprite, const sf::Time& time = sf::milliseconds(100))
    {
        if (frames_.size() <= 0) {
            currentFrameRemainingTime_ = time;
        }

        frames_.emplace_back(time, sprite);
    }

    inline sf::Sprite GetFrame(std::size_t i) const { return frames_[i].sprite; }

    inline std::size_t GetFrameCount() const { return frames_.size(); }
};

