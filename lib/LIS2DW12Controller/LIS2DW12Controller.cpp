#include "LIS2DW12Controller.hpp"
#define I2C_MASTER_SCL_IO GPIO_NUM_22 /*!< GPIO number used for I2C master clock */
#define I2C_MASTER_SDA_IO GPIO_NUM_21 /*!< GPIO number used for I2C master data  */
#define I2C_MASTER_NUM 0              /*!< I2C master i2c port number, the number of i2c peripheral interfaces available will depend on the chip */
#define I2C_MASTER_FREQ_HZ 400000     /*!< I2C master clock frequency */
#define I2C_MASTER_TX_BUF_DISABLE 0   /*!< I2C master doesn't need buffer */
#define I2C_MASTER_RX_BUF_DISABLE 0   /*!< I2C master doesn't need buffer */
#define I2C_MASTER_TIMEOUT_MS 1000
#define LIS2DW12_I2C_ADD 0x19U
static int32_t platform_write(void *handle, uint8_t reg, const uint8_t *bufp,
                              uint16_t len);
static int32_t platform_read(void *handle, uint8_t reg, uint8_t *bufp,
                             uint16_t len);

LIS2DW12Controller::LIS2DW12Controller()
{
    singleTapCallback = nullptr;
    doubleTapCallback = nullptr;
    newDataCallback = nullptr;
}

LIS2DW12Controller::~LIS2DW12Controller()
{
}

void LIS2DW12Controller::init()
{
    i2cPort = I2C_MASTER_NUM; // Change this to the appropriate I2C port
    uint8_t whoamI = 0, rst = 0;
    i2c_config_t i2cConf = {
        .mode = I2C_MODE_MASTER,
        .sda_io_num = I2C_MASTER_SDA_IO, // Change to the appropriate pins
        .scl_io_num = I2C_MASTER_SCL_IO,
        .sda_pullup_en = GPIO_PULLUP_DISABLE,
        .scl_pullup_en = GPIO_PULLUP_DISABLE,
        .master = {
            .clk_speed = I2C_MASTER_FREQ_HZ,
        },
        .clk_flags = 0,
    };

    ESP_LOGI(LOG_TAG, "i2c_param_config %d", i2c_param_config(i2cPort, &i2cConf));
    ESP_LOGI(LOG_TAG, "i2c_driver_install %d", i2c_driver_install(i2cPort, i2cConf.mode, I2C_MASTER_RX_BUF_DISABLE, I2C_MASTER_TX_BUF_DISABLE, 0));

    devCtx.write_reg = platform_write;
    devCtx.read_reg = platform_read;
    devCtx.handle = NULL;
    // Wait for sensor boot time
    platformDelay(bootTime);

    // Check device ID
    lis2dw12_device_id_get(&devCtx, &whoamI);
    while (whoamI != LIS2DW12_ID)
    {
        ESP_LOGE(LOG_TAG, "Not found. ID: 0x%X", whoamI);
        lis2dw12_device_id_get(&devCtx, &whoamI);
    }

    // Restore default configuration
    lis2dw12_reset_set(&devCtx, PROPERTY_ENABLE);
    do
    {
        lis2dw12_reset_get(&devCtx, &rst);
    } while (rst);

    initDoubleTap();

    ESP_LOGI(LOG_TAG, "Init done");
}

void LIS2DW12Controller::initDoubleTap()
{
    lis2dw12_reg_t intRoute;

    // Set full scale
    lis2dw12_full_scale_set(&devCtx, LIS2DW12_2g);

    // Configure power mode
    lis2dw12_power_mode_set(&devCtx, LIS2DW12_CONT_LOW_PWR_LOW_NOISE_12bit);

    // Set Output Data Rate
    lis2dw12_data_rate_set(&devCtx, LIS2DW12_XL_ODR_800Hz);

    // Enable Tap detection on X, Y, Z
    lis2dw12_tap_detection_on_z_set(&devCtx, PROPERTY_ENABLE);
    lis2dw12_tap_detection_on_y_set(&devCtx, PROPERTY_ENABLE);
    lis2dw12_tap_detection_on_x_set(&devCtx, PROPERTY_ENABLE);

    // Set Tap threshold on all axis
    lis2dw12_tap_threshold_x_set(&devCtx, 24);
    lis2dw12_tap_threshold_y_set(&devCtx, 24);
    lis2dw12_tap_threshold_z_set(&devCtx, 24);

    // Configure Double Tap parameter
    lis2dw12_tap_dur_set(&devCtx, 1);
    lis2dw12_tap_quiet_set(&devCtx, 3);
    lis2dw12_tap_shock_set(&devCtx, 3);

    // Enable Double Tap detection
    lis2dw12_tap_mode_set(&devCtx, LIS2DW12_BOTH_SINGLE_DOUBLE);

    // Enable single tap detection interrupt
    lis2dw12_pin_int1_route_get(&devCtx, &intRoute.ctrl4_int1_pad_ctrl);
    intRoute.ctrl4_int1_pad_ctrl.int1_tap = PROPERTY_ENABLE;
    lis2dw12_pin_int1_route_set(&devCtx, &intRoute.ctrl4_int1_pad_ctrl);
}

void LIS2DW12Controller::registerSingleTapCallback(const TapCallback &callback)
{
    singleTapCallback = callback;
}

void LIS2DW12Controller::registerDoubleTapCallback(const TapCallback &callback)
{
    doubleTapCallback = callback;
}

void LIS2DW12Controller::task()
{

    lis2dw12_all_sources_t allSource;
    memset(&allSource, 0, sizeof(allSource));

    t_lis2dw12_event_data event_data;
    memset(&event_data, 0, sizeof(t_lis2dw12_event_data));

    // Check Double Tap events
    lis2dw12_all_sources_get(&devCtx, &allSource);
    lis2dw12_acceleration_raw_get(&devCtx, event_data.accData);

    if (allSource.tap_src.double_tap)
    {
        event_data.isDoubleTap = true;
        ESP_LOGI(LOG_TAG, "Double Tap Detected: Sign %s", allSource.tap_src.tap_sign ? "positive" : "negative");

        if (allSource.tap_src.x_tap)
        {
            ESP_LOGI(LOG_TAG, " on X");
            event_data.xTap = true;
        }

        if (allSource.tap_src.y_tap)
        {
            ESP_LOGI(LOG_TAG, " on Y");
            event_data.yTap = true;
        }

        if (allSource.tap_src.z_tap)
        {
            ESP_LOGI(LOG_TAG, " on Z");
            event_data.zTap = true;
        }

        if (doubleTapCallback)
        {
            doubleTapCallback(event_data);
        }
    }

    // Check Single Tap events
    if (allSource.tap_src.single_tap)
    {
        ESP_LOGI(LOG_TAG, "Tap Detected: Sign %s", allSource.tap_src.tap_sign ? "positive" : "negative");
        event_data.isSingleTap = true;
        if (allSource.tap_src.x_tap)
        {
            ESP_LOGI(LOG_TAG, " on X");
            event_data.xTap = true;
        }

        if (allSource.tap_src.y_tap)
        {
            ESP_LOGI(LOG_TAG, " on Y");
            event_data.yTap = true;
        }

        if (allSource.tap_src.z_tap)
        {
            ESP_LOGI(LOG_TAG, " on Z");
            event_data.zTap = true;
        }

        if (singleTapCallback)
        {
            singleTapCallback(event_data);
        }
    }

    if (newDataCallback)
    {
        newDataCallback(event_data);
    }
}

// Static delay function
void LIS2DW12Controller::platformDelay(uint32_t ms)
{
    vTaskDelay(ms / portTICK_PERIOD_MS);
}

static int32_t platform_write(void *handle, uint8_t reg, const uint8_t *bufp,
                              uint16_t len)
{
    static uint8_t tx_buffer[256] = {0};
    if (len > 255)
        return ESP_FAIL;
    tx_buffer[0] = reg;
    memcpy(&tx_buffer[1], bufp, len);

    return i2c_master_write_to_device(I2C_MASTER_NUM, LIS2DW12_I2C_ADD, tx_buffer, len + 1, I2C_MASTER_TIMEOUT_MS / portTICK_PERIOD_MS);
}

static int32_t platform_read(void *handle, uint8_t reg, uint8_t *bufp,
                             uint16_t len)
{
    return i2c_master_write_read_device(I2C_MASTER_NUM, LIS2DW12_I2C_ADD, &reg, 1, bufp, len, I2C_MASTER_TIMEOUT_MS / portTICK_PERIOD_MS);
}