#pragma once
#include "Animation.hpp"

class HueColorAnimation : public Animation 
{
private:
    uint32_t _hue = 0;
protected:
    void animate(uint32_t currentTime);
    void reset(void);
public:
    HueColorAnimation(LedController &ledController, uint32_t updateTime,uint32_t hue, std::string name = "HueColorAnimation");
};


