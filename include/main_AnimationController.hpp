#pragma once


#include "LedController.hpp"
#include "AnimationController.hpp"
#include "RainbowAnimation.hpp"
#include "HueColorAnimation.hpp"
#include "RainbowAnimationFlow.hpp"
#include "esp_log.h"
#include <vector>

extern AnimationController animationController;

void animation_controller_task(void *pvParameters);
