#include "main_gpio.hpp"
#include "esp_log.h"
#include "main_led.hpp"
#include "main_esp_now.hpp"
#include "main_AnimationController.hpp"

InputController inputController;

void setup_gpio(void)
{
    inputController.setup();
}

void gpio_task(void *pvParameter)
{
    while(1) {
        inputController.update();
        delay_ms(10);
    }
}
