#pragma once

#include <stdint.h>
#include <stdbool.h>

#define delay_ms(ms) vTaskDelay((ms) / portTICK_RATE_MS)

extern "C" void led_init(void);
void led_set_duty(float duty);
void led_task(void *pvParameters);
void toggle_led(void);
void set_led(bool enable);
bool is_led_enabled(void);
