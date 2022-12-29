#include "main_lis2dw12.hpp"

LIS2DW12Controller lis2dw12Controller;

void main_lis2dw12_init(void)
{
   lis2dw12Controller.init();
}
void main_list2dw12_task(void *pvParameters)
{
    while (1)
    {
        lis2dw12Controller.task();
        vTaskDelay(10 / portTICK_PERIOD_MS);
    }
}
