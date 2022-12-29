#pragma once
#include "LedController.hpp"
#include <string>
#include <functional>

typedef struct __attribute__ ((packed)) {
    uint32_t updateTime;
    char name[32];  // Assuming the name field is a string with a maximum length of 32 characters
} t_animation_ble_config;

class Animation
{
private:
    std::string _name;
    uint32_t _updateTime;
    uint32_t _lastUpdateTime;
    std::function<void(uint32_t)> _animate;
    std::function<void()> _onReset;
protected:
    LedController *_ledController;
   
public:
    Animation(LedController &ledController, uint32_t updateTime, std::string name, std::function<void(uint32_t)> animate,std::function<void()> onReset);
    ~Animation();

    std::string getName();

    
    void reset(void){
        return _onReset();
    }

    void update(uint32_t currentTime);

    t_animation_ble_config getConfig();
};
