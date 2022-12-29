// RainbowAnimation.cpp
#include "RainbowAnimation.hpp"

RainbowAnimation::RainbowAnimation(LedController &ledController, uint32_t updateTime, std::string name)
    : Animation(ledController, updateTime, name, [this](uint32_t currentTime) { this->animate(currentTime); },[this]() { this->reset(); })
{
    // Initialize _hue in the initialization list
    _hue = 0;
}
void RainbowAnimation::animate(uint32_t currentTime)
{
    _hue++;
    _ledController->setAllHSV(_hue, 100, 100);
    _ledController->refresh();
}

void RainbowAnimation::reset()
{
    _hue = 0;
}