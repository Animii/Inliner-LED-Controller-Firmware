#pragma once
#include "Animation.hpp"

class RainbowAnimationFlow : public Animation 
{
private:
    uint32_t _hue = 0;
    uint32_t _hueStep = 0;
protected:
    void animate(uint32_t currentTime);
    void reset(void);
public:
    RainbowAnimationFlow(LedController &ledController, uint32_t updateTime, uint32_t hueSteps = 5, std::string name = "RainbowAnimationFlow");
};