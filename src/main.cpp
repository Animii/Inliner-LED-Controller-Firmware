/* LEDC (LED Controller) basic example
   This example code is in the Public Domain (or CC0 licensed, at your option.)
   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/
#define CONFIG_NIMBLE_CPP_LOG_LEVEL 4
#include <stdio.h>
#include "driver/uart.h"
#include "esp_err.h"
#include "driver/ledc.h"
#include <string.h>
#include "esp_log.h"
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <soc/rmt_struct.h>
#include <esp_system.h>
#include <nvs_flash.h>
#include <driver/gpio.h>
#include "ws2812.h"
#include "esp_err.h"
#include "driver/i2c.h"
#include "main_led.hpp"
#include "main_AnimationController.hpp"
#include "main_gpio.hpp"
#include "main_esp_now.hpp"
#include "main_lis2dw12.hpp"
#include "main_ble.hpp"
#include "main_deviceController.h"
#include "main_ble.hpp"
extern "C" void app_main(void)
{
    ESP_LOGI("MAIN", "Starting");
    // Set the LEDC peripheral configuration
    // led_init();

    main_lis2dw12_init();
    setup_gpio();
    nvs_flash_init();
    esp_now_setup();
    main_ble_init();

    main_deviceController_setup(animationController,EspNowController::getInstance(),inputController,lis2dw12Controller,bleController);

    // xTaskCreate(led_task, "led_task", 4096, NULL, 10, NULL);
    //xTaskCreate(main_list2dw12_task, "main_list2dw12_task", 4096, NULL, 5, NULL);
    xTaskCreate(gpio_task, "gpio_task", 4096, NULL, 0, NULL);
    xTaskCreatePinnedToCore(animation_controller_task, "animation_controller_task", 4096, NULL, 10, NULL,1);
    xTaskCreate(main_deviceController_task, "deviceController_task", 4096, NULL, 0, NULL);
}