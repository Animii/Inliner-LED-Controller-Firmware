#pragma once
#include "InputController.hpp"

extern InputController inputController;

void setup_gpio(void);
void gpio_task(void *pvParameter);
