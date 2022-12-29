#include "HueColorAnimation.hpp"
 HueColorAnimation::HueColorAnimation(LedController &ledController, uint32_t updateTime,uint32_t hue, std::string name) : Animation(ledController, updateTime, name, [this](uint32_t currentTime) { this->animate(currentTime); },[this]() { this->reset(); })
    {
        this->_hue = hue;
    }
void HueColorAnimation::animate(uint32_t currentTime) {
    this->_ledController->setAllHSV(this->_hue, 100, 100);
    this->_ledController->refresh();
}

void HueColorAnimation::reset(void)
{}