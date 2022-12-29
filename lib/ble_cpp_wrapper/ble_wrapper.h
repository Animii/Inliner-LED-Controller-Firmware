#ifndef BLE_WRAPPER_H
#define BLE_WRAPPER_H

#include "esp_gatts_api.h"
#include "esp_gap_ble_api.h"
#include "esp_bt_main.h"
#include "esp_bt_defs.h"
#include "esp_bt.h"

class BleWrapper {
public:
    BleWrapper();

    void init();
    void create_gatt_server();

private:
    static void gatts_event_handler(esp_gatts_cb_event_t event, esp_gatt_if_t gatts_if, esp_ble_gatts_cb_param_t *param);
    static void gap_event_handler(esp_gap_ble_cb_event_t event, esp_ble_gap_cb_param_t *param);

    static const uint16_t GATTS_SERVICE_UUID_TEST;
    static const uint16_t GATTS_CHAR_UUID_TEST;
    static const uint16_t GATTS_NUM_HANDLE_TEST;

    static uint16_t gatts_service_handle;
    static esp_gatt_if_t gatts_if;
};

#endif // BLE_WRAPPER_H
