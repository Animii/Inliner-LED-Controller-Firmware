#include "LedController.hpp"
#include "driver/rmt.h"
#include <string>
#include "esp_log.h"

#define RMT_TX_CHANNEL RMT_CHANNEL_0
static const std::string TAG = "LedController";
LedController::LedController(uint32_t ledCount, gpio_num_t ledPin,float brightness)
{
    this->_ledCount = ledCount;
    this->_ledPin = ledPin;
    this->setBrightness(brightness);
}

LedController::~LedController()
{
}

esp_err_t LedController::init(void)
{
    rmt_config_t config = RMT_DEFAULT_CONFIG_TX(this->_ledPin, RMT_TX_CHANNEL);
    // set counter clock to 40MHz
    config.clk_div = 2;

    ESP_ERROR_CHECK(rmt_config(&config));
    ESP_ERROR_CHECK(rmt_driver_install(config.channel, 0, 0));

    // install ws2812 driver
    led_strip_config_t strip_config = LED_STRIP_DEFAULT_CONFIG(this->_ledCount, (led_strip_dev_t)config.channel);
    this->_strip = led_strip_new_rmt_ws2812(&strip_config);
    if (!this->_strip)
    {
        ESP_LOGE(TAG.c_str(), "install WS2812 driver failed");
    }
    // Clear LED strip (turn off all LEDs)
    ESP_ERROR_CHECK(this->_strip->clear(this->_strip, 100));
    return ESP_OK;
}

esp_err_t LedController::deinit(void)
{
    return ESP_OK;
}

void LedController::setBrightness(float brightness)
{
    if(brightness > 1.0f) brightness = 1.0;
    this->_brightness = brightness;
}

float LedController::getBrightness(void)
{
   return this->_brightness;
}

esp_err_t LedController::setAllRGB(uint32_t red, uint32_t green, uint32_t blue)
{
    for (uint32_t i = 0; i < this->_ledCount; i++)
    {
        ESP_ERROR_CHECK(this->setPixelRGB(i, red, green, blue));
    }
    return ESP_OK;
}

esp_err_t LedController::setAllHSV(uint32_t h, uint32_t s, uint32_t v)
{
    for (uint32_t i = 0; i < this->_ledCount; i++)
    {
        ESP_ERROR_CHECK(this->setPixelHSV(i, h, s, v));
    }

    return ESP_OK;
}

esp_err_t LedController::setPixelRGB(uint32_t index, uint32_t red, uint32_t green, uint32_t blue)
{
    this->_strip->set_pixel(this->_strip, index, red * this->_brightness, green * this->_brightness, blue * this->_brightness);
    return ESP_OK;
}

esp_err_t LedController::setPixelHSV(uint32_t index, uint32_t h, uint32_t s, uint32_t v)
{
    uint32_t r, g, b;
    this->hsvTorgb(h, s, v, &r, &g, &b);
    this->setPixelRGB(index,r ,g, b );
    return ESP_OK;
}

esp_err_t LedController::refresh(void)
{
    ESP_ERROR_CHECK(this->_strip->refresh(this->_strip, 100));
    return ESP_OK;
}

void LedController::hsvTorgb(uint32_t h, uint32_t s, uint32_t v, uint32_t *r, uint32_t *g, uint32_t *b)
{
    h %= 360; // h -> [0,360]
    uint32_t rgb_max = v * 2.55f;
    uint32_t rgb_min = rgb_max * (100 - s) / 100.0f;

    uint32_t i = h / 60;
    uint32_t diff = h % 60;

    // RGB adjustment amount by hue
    uint32_t rgb_adj = (rgb_max - rgb_min) * diff / 60;

    switch (i)
    {
    case 0:
        *r = rgb_max;
        *g = rgb_min + rgb_adj;
        *b = rgb_min;
        break;
    case 1:
        *r = rgb_max - rgb_adj;
        *g = rgb_max;
        *b = rgb_min;
        break;
    case 2:
        *r = rgb_min;
        *g = rgb_max;
        *b = rgb_min + rgb_adj;
        break;
    case 3:
        *r = rgb_min;
        *g = rgb_max - rgb_adj;
        *b = rgb_max;
        break;
    case 4:
        *r = rgb_min + rgb_adj;
        *g = rgb_min;
        *b = rgb_max;
        break;
    default:
        *r = rgb_max;
        *g = rgb_min;
        *b = rgb_max - rgb_adj;
        break;
    }
}

uint32_t LedController::getLEDCount(void){
    return this->_ledCount;
}