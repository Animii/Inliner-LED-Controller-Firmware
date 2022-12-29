#pragma once
#include "LIS2DW12Controller.hpp"

extern LIS2DW12Controller lis2dw12Controller;

void main_lis2dw12_init(void);
void main_list2dw12_task(void *pvParameters);