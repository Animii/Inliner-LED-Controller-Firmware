#pragma once
#include <stdint.h>

#include "esp_err.h"
#include "led_strip.h"
#include "soc/soc_caps.h"
#include "driver/gpio.h"
#include "freertos/FreeRTOS.h"
#include "freertos/ringbuf.h"
#include "soc/rmt_struct.h"
#include "hal/rmt_types.h"

class LedController
{
private:
    /* data */
    uint32_t _ledCount;
    float _brightness;
    gpio_num_t _ledPin;
    led_strip_t *_strip;

    void hsvTorgb(uint32_t h, uint32_t s, uint32_t v, uint32_t *r, uint32_t *g, uint32_t *b);

public:
    LedController(uint32_t ledCount, gpio_num_t ledPin, float brightness = 1);
    ~LedController();

    esp_err_t init(void);
    esp_err_t deinit(void);

    float getBrightness(void);
    void setBrightness(float brightness);

    esp_err_t setPixelRGB(uint32_t index, uint32_t red, uint32_t green, uint32_t blue);
    esp_err_t setPixelHSV(uint32_t index, uint32_t h, uint32_t s, uint32_t v);

    esp_err_t refresh(void);

    esp_err_t setAllRGB(uint32_t red, uint32_t green, uint32_t blue);
    esp_err_t setAllHSV(uint32_t h, uint32_t s, uint32_t v);

    uint32_t getLEDCount(void);
};
