#pragma once
#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"
#include "driver/gpio.h"
#include "esp_log.h"
#include <vector>
#include <functional> // Include the functional library for std::function

class InputController {
public:
    InputController();
    ~InputController();

    void setup();
    void update();

    void configureGPIO(gpio_num_t gpioNum, std::function<void()> callback);

private:
    static void IRAM_ATTR isrHandler(void* arg);

    const char* TAG = "InputController";
    static constexpr uint32_t debounceTimeout = 50;
    static QueueHandle_t gpioEventQueue;
    static uint32_t lastDebounceTime;

    struct GPIOConfig {
        gpio_num_t gpioNum;
        std::function<void()> callback;
    };

    std::vector<GPIOConfig> configuredGPIOs;
};

