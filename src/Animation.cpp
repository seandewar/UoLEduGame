#include "Animation.h"

#include "Game.h"


Animation::Animation() :
currentFrame_(0)
{
}


Animation::~Animation()
{
}


void Animation::Tick()
{
    currentFrameRemainingTime_ -= Game::FrameTimeStep;

    if (currentFrameRemainingTime_ <= sf::Time::Zero) {
        if (++currentFrame_ >= frames_.size()) {
            currentFrame_ = 0;
        }

        currentFrameRemainingTime_ += frames_[currentFrame_].time;
    }
}