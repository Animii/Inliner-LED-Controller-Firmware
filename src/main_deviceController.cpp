#include "main_deviceController.h"

DeviceController *deviceController;

void main_deviceController_setup(AnimationController &animationController, EspNowController &espNowController,InputController &inputController,LIS2DW12Controller &lis2dw12Controller, BLEController &bleController)
{
    deviceController = new DeviceController(animationController, espNowController, inputController, lis2dw12Controller,bleController);
    deviceController->init();
}

void main_deviceController_task(void *pvParameters)
{
    while (1)
    {
        deviceController->update();
        vTaskDelay(10 / portTICK_PERIOD_MS);
    }
}