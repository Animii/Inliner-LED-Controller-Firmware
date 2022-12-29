#include "main_led.hpp"
#include "driver/ledc.h"
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#define LEDC_TIMER LEDC_TIMER_0
#define LEDC_MODE LEDC_LOW_SPEED_MODE
#define LEDC_OUTPUT_IO (2) // Define the output GPIO
#define LEDC_CHANNEL LEDC_CHANNEL_0
#define LEDC_DUTY_RES LEDC_TIMER_13_BIT // Set duty resolution to 13 bits
#define LEDC_DUTY (4095)                // Set duty to 50%. ((2 ** 13) - 1) * 50% = 4095
#define LEDC_FREQUENCY (5000)           // Frequency in Hertz. Set frequency at 5 kHz
#define delay_ms(ms) vTaskDelay((ms) / portTICK_RATE_MS)

static uint32_t calc_duty(float duty);
static bool is_enabled = true;

void toggle_led(void)
{
    is_enabled = !is_enabled;
}

bool is_led_enabled(void){
    return is_enabled;
}

void set_led(bool enable)
{
    is_enabled = enable;
}
void led_init(void)
{
    /*
    // Prepare and then apply the LEDC PWM timer configuration
    ledc_timer_config_t ledc_timer = {
        .speed_mode = LEDC_MODE,
        .timer_num = LEDC_TIMER,
        .duty_resolution = LEDC_DUTY_RES,
        .freq_hz = LEDC_FREQUENCY, // Set output frequency at 5 kHz
        .clk_cfg = LEDC_AUTO_CLK};
    ESP_ERROR_CHECK(ledc_timer_config(&ledc_timer));

    // Prepare and then apply the LEDC PWM channel configuration
    ledc_channel_config_t ledc_channel = {
        .speed_mode = LEDC_MODE,
        .channel = LEDC_CHANNEL,
        .timer_sel = LEDC_TIMER,
        .intr_type = LEDC_INTR_DISABLE,
        .gpio_num = LEDC_OUTPUT_IO,
        .duty = 0, // Set duty to 0%
        .hpoint = 0};
    ESP_ERROR_CHECK(ledc_channel_config(&ledc_channel));
    */
}

void led_set_duty(float duty)
{
    //ESP_ERROR_CHECK(ledc_set_duty(LEDC_MODE, LEDC_CHANNEL, calc_duty(duty)));
    // Update duty to apply the new value
    //ESP_ERROR_CHECK(ledc_update_duty(LEDC_MODE, LEDC_CHANNEL));
}

static uint32_t calc_duty(float duty)
{
    return (uint32_t)((0x01u << 13) - 1.0f) * duty;
}

void led_task(void *pvParameters)
{
    while (1)
    {
        if (is_enabled)
        {
            led_set_duty(0.5f);
            delay_ms(10);
        }else
        {
            led_set_duty(0.0f);
            delay_ms(10);
        }

    }
}