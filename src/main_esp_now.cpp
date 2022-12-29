#include "main_esp_now.hpp"
#include "main_led.hpp"
#include <stdlib.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"
#include "main_AnimationController.hpp"
#include "esp_log.h"
#include "esp_wifi.h"
#include "esp_netif.h"

#include "esp_now.h"

void esp_now_setup(void)
{
   EspNowController::getInstance().setup();
}
