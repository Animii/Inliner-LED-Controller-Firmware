#pragma once

#include "esp_log.h"
#include "driver/i2c.h"
#include "string.h"
#include "esp_err.h"
#include "lis2dw12_reg.h"
#include <vector>
#include <functional>

typedef struct __attribute__ ((packed)) {
    bool isSingleTap;
    bool isDoubleTap;

    uint8_t xTap;
    uint8_t yTap;
    uint8_t zTap;

    int16_t accData[3];
}t_lis2dw12_event_data;

class LIS2DW12Controller {
public:
    LIS2DW12Controller();
    ~LIS2DW12Controller();

    void init();
    void task();

    // Function types for callback registration
    using TapCallback = std::function<void(t_lis2dw12_event_data)>;
    using NewDataCallBack = std::function<void(t_lis2dw12_event_data)>;

    // Register callback functions for tap events
    void registerSingleTapCallback(const TapCallback& callback);
    void registerDoubleTapCallback(const TapCallback& callback);

private:
    static void platformDelay(uint32_t ms);
    const char* LOG_TAG = "LIS2DW12Controller";
    void initDoubleTap();

    static const uint32_t bootTime = 200; // ms
    static const uint8_t i2cAddress = 0x19U;

    stmdev_ctx_t devCtx;
    i2c_port_t i2cPort;

    // Callback functions for tap events
    TapCallback singleTapCallback;
    TapCallback doubleTapCallback;
    NewDataCallBack newDataCallback;
};
