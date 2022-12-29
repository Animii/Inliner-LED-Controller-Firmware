#include "AnimationController.hpp"

AnimationController::AnimationController(BLEController &bleController, LedController &ledController, std::vector<Animation *> animations) : BLEComponent(bleController,SERVICE_UUID,characteristics), _ledController(ledController)
{
    if (animations.size() > 0)
    {
        this->_animations = animations;
        this->_currentAnimationIndex = 0;
    }

    this->_state = STOPPED;

    characteristics[CHARACTERISTIC_PLAY_UUID] = [this](const uint8_t* data, size_t size) {
        if (size != sizeof(uint32_t))
            return;
        this->play(*(uint32_t*)data);
    };

    characteristics[CHARACTERISTIC_CURRENT_ANIMATION_UUID] = [this](const uint8_t* data, size_t size) {
    };

    characteristics[CHARACTERISTIC_BRIGHTNESS_UUID] = [this](const uint8_t* data, size_t size) {
        if(sizeof(this->_brightness) != size)
        {
            return;
        }

        this->setBrightness(data[0]);
    };

    characteristics[CHARACTERISTIC_ANIMATION_CONFIG_UUID] = [this](const uint8_t* data, size_t size){
    };
}

AnimationController::~AnimationController()
{
    
}



void AnimationController::update(uint32_t currentTime)
{
    if(!this->_enableTimeClock){
        return;
    }
    if (this->_state == RUNNING && this->_currentAnimationIndex >= 0)
    {
        this->_animations[this->_currentAnimationIndex]->update(currentTime);
    }
}

void AnimationController::start(void)
{
    this->_state = RUNNING;
}

void AnimationController::stop(void)
{
    this->_state = STOPPED;
}

void AnimationController::resetAnimation(void)
{
    if (this->_currentAnimationIndex >= 0)
    {
        this->_animations[this->_currentAnimationIndex]->reset();
    }
}

void AnimationController::notifyBleConfig(void)
{
   // Calculating the total size needed for the BLE notification payload
    size_t configSize = sizeof(t_animation_ble_config);
    size_t totalSize = sizeof(uint32_t) + (_animations.size() * configSize); // animationCount + (animationConfig * number of animations)
    
    // Creating a buffer to hold the BLE notification payload
    uint8_t *buffer = new uint8_t[totalSize];
    
    // Ensuring buffer allocation was successful
    if (!buffer)
    {
        // Handle error (e.g., log an error message)
        return;
    }
    
    // Writing the number of animations to the buffer
    uint32_t animationCount = static_cast<uint32_t>(_animations.size());
    memcpy(buffer, &animationCount, sizeof(uint32_t));
    
    // Iterating through all animations and writing their config to the buffer
    for (size_t i = 0; i < _animations.size(); ++i)
    {
        t_animation_ble_config config = _animations[i]->getConfig();
        
        // Safeguard to ensure the name is null-terminated
        if (config.name[31] != '\0')
        {
            // Optionally: Handle error (e.g., log an error message)
            config.name[31] = '\0';
        }

        // Copying the animation config into the buffer, offsetting by the previously written data
        memcpy(buffer + sizeof(uint32_t) + (i * configSize), &config, configSize);
    }
    
    // Notifying the BLE characteristic with the prepared data
    notify(CHARACTERISTIC_ANIMATION_CONFIG_UUID, buffer, totalSize);
    
    // Freeing the dynamically allocated buffer
    delete[] buffer;
}

void AnimationController::remove(uint32_t index)
{
    if (index < this->_animations.size())
    {
        this->_animations.erase(this->_animations.begin() + index);
        if (this->_currentAnimationIndex >= index)
        {
            prev();
        }
    }
}

void AnimationController::clear(void)
{
    this->_animations.clear();
    this->_currentAnimationIndex = -1;
}
void AnimationController::next(void)
{
    play(this->_currentAnimationIndex + 1);
}
void AnimationController::prev(void)
{
    play(this->_currentAnimationIndex - 1);
}
void AnimationController::  play(uint32_t index) {
    if (this->_animations.empty()) {
        return;
    }

    uint32_t clampedIndex = index % this->_animations.size();
    this->_currentAnimationIndex = clampedIndex;
    resetAnimation();
    t_animation_ble_config config = this->getCurrentAnimationConfig();
    t_animation_contoller_ble_state controllerState = this->getConfig();
    notify(CHARACTERISTIC_PLAY_UUID,(uint8_t*)&controllerState,sizeof(t_animation_contoller_ble_state));
    notify(CHARACTERISTIC_CURRENT_ANIMATION_UUID,(uint8_t*)&config,sizeof(t_animation_ble_config));
}

uint32_t AnimationController::getCurrentAnimationIndex(void)
{
    return this->_currentAnimationIndex;
}

void AnimationController::setBrightness(uint8_t brightness)
{
    this->_brightness = brightness;
    this->_ledController.setBrightness(brightness / 255.0f);
    notify(CHARACTERISTIC_BRIGHTNESS_UUID,&this->_brightness,sizeof(this->_brightness));
}
