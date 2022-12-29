// RainbowAnimation.hpp
#pragma once
#include "Animation.hpp"

class RainbowAnimation : public Animation 
{
private:
    uint32_t _hue = 0;
    
protected:
    void animate(uint32_t currentTime) ;
    void reset() ;
    
public:
    RainbowAnimation(LedController &ledController, uint32_t updateTime, std::string name = "RainbowAnimation");
};
