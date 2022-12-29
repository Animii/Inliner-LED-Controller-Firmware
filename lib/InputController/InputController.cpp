#include "InputController.hpp"
QueueHandle_t InputController::gpioEventQueue = nullptr;
uint32_t InputController::lastDebounceTime = 0;

InputController::InputController()
{
    gpioEventQueue = nullptr;
}

InputController::~InputController()
{
    if (gpioEventQueue != nullptr)
    {
        vQueueDelete(gpioEventQueue);
    }
}

void InputController::setup()
{
    gpioEventQueue = xQueueCreate(10, sizeof(uint32_t));
    // Additional setup as before
}

void InputController::update()
{
    gpio_num_t ioNum;

    if (xQueueReceive(gpioEventQueue, &ioNum, portMAX_DELAY))
    {
        for (const auto &config : configuredGPIOs)
        {
            if (config.gpioNum == ioNum && config.callback)
            {
                config.callback();
            }
        }
    }
}

void InputController::configureGPIO(gpio_num_t gpioNum, std::function<void()> callback)
{
    // Configure the GPIO pin
    gpio_set_direction(gpioNum, GPIO_MODE_INPUT);
    gpio_set_intr_type(gpioNum, GPIO_INTR_POSEDGE);
    gpio_install_isr_service(0);
    gpio_isr_handler_add(gpioNum, isrHandler, (void *)gpioNum);

    // Store the GPIO configuration for later use
    configuredGPIOs.push_back({gpioNum, callback});
}

void IRAM_ATTR InputController::isrHandler(void *arg)
{
    uint32_t gpioNum = (uint32_t)arg;
    uint32_t now = xTaskGetTickCount();
    if (now - debounceTimeout > lastDebounceTime)
    {
        lastDebounceTime = xTaskGetTickCount();
        xQueueSendFromISR(gpioEventQueue, &gpioNum, nullptr);
    }
}
