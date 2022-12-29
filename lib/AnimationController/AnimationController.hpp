#pragma once
#include <vector>

#include "LedController.hpp"
#include "Animation.hpp"
#include  "BLEComponent.hpp"

enum AnimationControllerState
{
    RUNNING,
    STOPPED
};
typedef struct __attribute__((packed))
{
    uint8_t state;
    uint32_t currentAnimationIndex;
} t_animation_contoller_ble_state;

typedef struct __attribute__((packed))
{
    uint32_t animationCount;
    t_animation_ble_config animationConfig[0];
} t_animation_contoller_ble_config;

class AnimationController : public BLEComponent
{
private:
    std::string SERVICE_UUID = "000000ff-0000-1000-8000-00805f9b34fb";
    std::string CHARACTERISTIC_PLAY_UUID = "0000ff02-0000-1000-8000-00805f9b34fb";
    std::string CHARACTERISTIC_CURRENT_ANIMATION_UUID = "0000ff03-0000-1000-8000-00805f9b34fb";
    std::string CHARACTERISTIC_BRIGHTNESS_UUID = "0000ff04-0000-1000-8000-00805f9b34fb";
    std::string CHARACTERISTIC_ANIMATION_CONFIG_UUID = "0000ff05-0000-1000-8000-00805f9b34fb";

    
    std::vector<Animation *> _animations;
    AnimationControllerState _state;
    bool _enableTimeClock = true;
    int32_t _currentAnimationIndex = -1;
    std::map<std::string, CharacteristicCallback> characteristics;
    uint8_t _brightness = 255;

public:
    AnimationController(BLEController &bleController, LedController &ledController, std::vector<Animation *> animations);
    ~AnimationController();
    LedController &_ledController;
    void add(Animation &animation);
    void remove(uint32_t index);
    void clear();



    void start(void);
    void stop(void);
    void resetAnimation(void);

    void notifyBleConfig(void);

    void update(uint32_t currentTime);
    void forceUpdate(uint32_t currentTime)
    {
        if (this->_state == RUNNING && this->_currentAnimationIndex >= 0)
        {
            this->_animations[this->_currentAnimationIndex]->update(currentTime);
        }
    }

    void setTimeClock(bool enable)
    {
        this->_enableTimeClock = enable;
    }

    AnimationControllerState getState(void);

    void next(void);
    void prev(void);
    void play(uint32_t index);

    uint32_t getCurrentAnimationIndex(void);

    void setBrightness(uint8_t brightness);

    t_animation_contoller_ble_state getConfig()
    {
        t_animation_contoller_ble_state config;
        config.state = static_cast<uint8_t>(_state);
        config.currentAnimationIndex = _currentAnimationIndex;
        return config;
    }

    t_animation_ble_config getCurrentAnimationConfig()
    {
        return _animations[this->getCurrentAnimationIndex()]->getConfig();
    }
};
