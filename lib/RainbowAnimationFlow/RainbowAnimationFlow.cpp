#include "RainbowAnimationFlow.hpp"

RainbowAnimationFlow::RainbowAnimationFlow(LedController &ledController, uint32_t updateTime, uint32_t hueSteps, std::string name)
    : Animation(ledController, updateTime, name, [this](uint32_t currentTime) { this->animate(currentTime); },[this]() { this->reset(); })
{
    this->_hueStep = hueSteps;
}

void RainbowAnimationFlow::animate(uint32_t currentTime)
{
    this->_hue++;
    for (size_t i = 0; i < this->_ledController->getLEDCount(); i++)
    {
        this->_ledController->setPixelHSV(i, this->_hue + (i * 5), 100, 100);
    }
    this->_ledController->refresh();
}

void RainbowAnimationFlow::reset(void)
{
    this->_hue = 0;
}
