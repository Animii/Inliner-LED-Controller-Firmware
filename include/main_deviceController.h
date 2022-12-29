#pragma once
#include "DeviceController.hpp"


void main_deviceController_setup(AnimationController &animationController, EspNowController &espNowController,InputController &inputController,LIS2DW12Controller &lis2dw12Controller, BLEController &bleController);

void main_deviceController_task(void *pvParameters);