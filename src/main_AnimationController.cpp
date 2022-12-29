#include "main_AnimationController.hpp"
#include "main_ble.hpp"
#define delay_ms(ms) vTaskDelay((ms) / portTICK_RATE_MS)

char *TAG = "main_animation_controller_task";
#define WS2812_PIN (gpio_num_t)15

LedController ledController = {64 * 5, WS2812_PIN, 1.0f};
static uint32_t lastColorChangeTime = 0;
static Animation policeAnimation = Animation(ledController, 10, "PoliceAnimation",
    [](uint32_t currentTime) {
        static bool isRedOn = true;
        // Toggle between red and blue every 500 milliseconds
        if (currentTime - lastColorChangeTime >= 500) {
            lastColorChangeTime = currentTime;
            isRedOn = !isRedOn;
        }

        // Set the LED colors based on the state
        if (isRedOn) {
            // Set LEDs to red
            for (size_t i = 0; i < ledController.getLEDCount(); i++) {
                ledController.setPixelRGB(i, 255, 0, 0);  // Red
            }
        } else {
            // Set LEDs to blue
            for (size_t i = 0; i < ledController.getLEDCount(); i++) {
                ledController.setPixelRGB(i, 0, 0, 255);  // Blue
            }
        }

        // Refresh the LED controller
        ledController.refresh();
    },
    []() {
        lastColorChangeTime = 0;
    });
static Animation policeAnimation2 = Animation(ledController, 50, "PoliceAnimation2",
    [](uint32_t currentTime) {
        static int colorIndex = 0;
        static const int numColors = 3;
        static const uint32_t colorDurations[numColors] = {200, 200, 200}; // Time in milliseconds for each color
        

        // Get the current time
        auto current_time = currentTime;

        // Calculate the time elapsed in milliseconds
        auto elapsed_time = current_time;

        // Change color every colorDuration milliseconds
        if (elapsed_time - lastColorChangeTime >= colorDurations[colorIndex]) {
            lastColorChangeTime = elapsed_time;
            colorIndex = (colorIndex + 1) % numColors;
        }

        // Set the LED colors based on the current color
        if (colorIndex == 0) {
            // Red
            for (size_t i = 0; i < ledController.getLEDCount(); i++) {
                ledController.setPixelRGB(i, 255, 0, 0);
            }
        } else if (colorIndex == 1) {
            // Blue
            for (size_t i = 0; i < ledController.getLEDCount(); i++) {
                ledController.setPixelRGB(i, 0, 0, 255);
            }
        } else {
            // White
            for (size_t i = 0; i < ledController.getLEDCount(); i++) {
                ledController.setPixelRGB(i, 255, 255, 255);
            }
        }

        // Refresh the LED controller
        ledController.refresh();
    },
    []() {
       lastColorChangeTime = 0;
    });

std::vector<Animation *> animations = {
    new RainbowAnimation(ledController, 10),
    new RainbowAnimationFlow(ledController, 10, 10),
    new HueColorAnimation(ledController, 10, 120),
    new HueColorAnimation(ledController, 10, 0),
    new HueColorAnimation(ledController, 10, 240),
    &policeAnimation,
    &policeAnimation2
};

AnimationController animationController = {bleController, ledController, animations};

void animation_controller_task(void *pvParameters)
{
    ESP_LOGI(TAG, "animation_controller_task started");

    ledController.init();
    ledController.setAllRGB(1, 1, 1);
    ledController.refresh();
    animationController.start();
    while (1)
    {
        //animationController.update(xTaskGetTickCount());
        delay_ms(5);
    }
}