#include "Animation.hpp"
#include <stdexcept>
#include <stdio.h>
#include <string.h>
Animation::Animation(LedController &ledController, uint32_t updateTime, std::string name, std::function<void(uint32_t)> animate,std::function<void()> onReset)
{
    if (name.length() > 30)
    {
        throw std::invalid_argument("The max length of name is 30");
    }

    this->_animate = animate;
    this->_onReset = onReset;
    this->_ledController = &ledController;
    this->_updateTime = updateTime;
    this->_lastUpdateTime = 0;
    this->_name = name;
}

Animation::~Animation()
{
    
}

std::string Animation::getName()
{
    return this->_name;
}


t_animation_ble_config Animation::getConfig()
{
    t_animation_ble_config config;
    config.updateTime = _updateTime;
    strncpy(config.name, _name.c_str(), sizeof(config.name) - 1);
    config.name[sizeof(config.name) - 1] = '\0'; // Ensure null-termination
    
    return config;
}

void Animation::update(uint32_t currentTime)
{
    if (currentTime - this->_lastUpdateTime >= this->_updateTime)
    {
        this->_lastUpdateTime = currentTime;
        if(_animate)
        {
            _animate(currentTime);
        }
    }
}