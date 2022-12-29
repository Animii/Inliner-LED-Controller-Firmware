#include "ble_wrapper.h"

const uint16_t BleWrapper::GATTS_SERVICE_UUID_TEST = 0x00FF;
const uint16_t BleWrapper::GATTS_CHAR_UUID_TEST = 0xFF01;
const uint16_t BleWrapper::GATTS_NUM_HANDLE_TEST = 4;

uint16_t BleWrapper::gatts_service_handle = 0;
esp_gatt_if_t BleWrapper::gatts_if = 0;

BleWrapper::BleWrapper() {}

void BleWrapper::init() {
    esp_err_t ret;

    // Initialize Bluetooth controller
    esp_bt_controller_config_t bt_cfg = BT_CONTROLLER_INIT_CONFIG_DEFAULT();
    ret = esp_bt_controller_init(&bt_cfg);
    if (ret) {
        // Handle error
        return;
    }

    ret = esp_bt_controller_enable(ESP_BT_MODE_BLE);
    if (ret) {
        // Handle error
        return;
    }

    ret = esp_bluedroid_init();
    if (ret) {
        // Handle error
        return;
    }

    ret = esp_bluedroid_enable();
    if (ret) {
        // Handle error
        return;
    }

    ret = esp_ble_gatts_register_callback(gatts_event_handler);
    if (ret) {
        // Handle error
        return;
    }

    ret = esp_ble_gap_register_callback(gap_event_handler);
    if (ret) {
        // Handle error
        return;
    }

    ret = esp_ble_gatts_app_register(0);
    if (ret) {
        // Handle error
        return;
    }
}

void BleWrapper::create_gatt_server() {
    esp_err_t ret;

    esp_gatts_attr_db_t gatt_db[] = {
        // Service Declaration
        {
            {ESP_GATT_AUTO_RSP},
            {ESP_UUID_LEN_16, (uint8_t *)&GATTS_SERVICE_UUID_TEST, ESP_GATT_PERM_READ, sizeof(uint16_t), sizeof(GATTS_SERVICE_UUID_TEST), (uint8_t *)&GATTS_SERVICE_UUID_TEST}
        },

        // Characteristic Declaration
        {
            {ESP_GATT_AUTO_RSP},
            {ESP_UUID_LEN_16, (uint8_t *)&GATTS_CHAR_UUID_TEST, ESP_GATT_PERM_READ, sizeof(uint16_t), sizeof(GATTS_CHAR_UUID_TEST), (uint8_t *)&GATTS_CHAR_UUID_TEST}
        }
    };

    ret = esp_ble_gatts_create_attr_tab(gatt_db, gatts_if, GATTS_NUM_HANDLE_TEST, 0);
    if (ret) {
        // Handle error
        return;
    }
}

void BleWrapper::gatts_event_handler(esp_gatts_cb_event_t event, esp_gatt_if_t gatts_if, esp_ble_gatts_cb_param_t *param) {
    // Handle GATT server events
    switch (event) {
        case ESP_GATTS_REG_EVT:
            ESP_LOGI(TAG, "ESP_GATTS_REG_EVT");
            esp_ble_gap_set_device_name("ESP32-BLE");

            // Configure the adv_data and scan_rsp_data
            esp_ble_adv_data_t adv_data;
            esp_ble_adv_data_t scan_rsp_data;
            memset(&adv_data, 0, sizeof(adv_data));
            memset(&scan_rsp_data, 0, sizeof(scan_rsp_data));

            adv_data.set_scan_rsp = false;
            adv_data.include_name = true;
            adv_data.include_txpower = true;
            adv_data.min_interval = 0x20;
            adv_data.max_interval = 0x40;
            adv_data.appearance = 0x00;
            adv_data.manufacturer_len = 0;
            adv_data.p_manufacturer_data = NULL;
            adv_data.service_data_len = 0;
            adv_data.p_service_data = NULL;
            adv_data.service_uuid_len = 0;
            adv_data.p_service_uuid = NULL;
            adv_data.flag = (ESP_BLE_ADV_FLAG_GEN_DISC | ESP_BLE_ADV_FLAG_BREDR_NOT_SPT);

            scan_rsp_data.set_scan_rsp = true;
            scan_rsp_data.include_name = true;
            scan_rsp_data.include_txpower = true;
            scan_rsp_data.min_interval = 0x20;
            scan_rsp_data.max_interval = 0x40;
            scan_rsp_data.appearance = 0x00;
            scan_rsp_data.manufacturer_len = 0;
            scan_rsp_data.p_manufacturer_data = NULL;
            scan_rsp_data.service_data_len = 0;
            scan_rsp_data.p_service_data = NULL;
            scan_rsp_data.service_uuid_len = 0;
            scan_rsp_data.p_service_uuid = NULL;
            scan_rsp_data.flag = (ESP_BLE_ADV_FLAG_GEN_DISC | ESP_BLE_ADV_FLAG_BREDR_NOT_SPT);

            // Set the adv_data and scan_rsp_data
            esp_ble_gap_config_adv_data(&adv_data);
            esp_ble_gap_config_adv_data(&scan_rsp_data);

            break;

        case ESP_GATTS_CREAT_ATTR_TAB_EVT:
            if (param->add_attr_tab.status != ESP_GATT_OK) {
                ESP_LOGE(TAG, "Create attribute table failed, error code=0x%x", param->add_attr_tab.status);
            } else if (param->add_attr_tab.num_handle != GATTS_NUM_HANDLE_TEST) {
                ESP_LOGE(TAG, "Create attribute table: The number of handles created (%d) is not as expected (%d)", param->add_attr_tab.num_handle, GATTS_NUM_HANDLE_TEST);
            } else {
                ESP_LOGI(TAG, "Create attribute table: Handle table size %d", param->add_attr_tab.num_handle);
                memcpy(gatts_service_handle_table, param->add_attr_tab.handles, sizeof(gatts_service_handle_table));
                gatts_service_handle = param->add_attr_tab.handles[0];
                esp_ble_gatts_start_service(gatts_service_handle);
            }
            break;

        case ESP_GATTS_START_EVT:
            ESP_LOGI(TAG, "ESP_GATTS_START_EVT");
            break;

        case ESP_GATTS_STOP_EVT:
            ESP_LOGI(TAG, "ESP_GATTS_STOP_EVT");
            break;

        case ESP_GATTS_CONNECT_EVT:
            ESP_LOGI(TAG, "ESP_GATTS_CONNECT_EVT, conn_id: %d", param->connect.conn_id);
            break;

        case ESP_GATTS_DISCONNECT_EVT:
            ESP_LOGI(TAG, "ESP_GATTS_DISCONNECT_EVT");
            esp_ble_gap_start_advertising(ESP_BLE_ADV_PARAMS_DEFAULT);
            break;

        case ESP_GATTS_READ_EVT:
            ESP_LOGI(TAG, "ESP_GATTS_READ_EVT");
            break;

        case ESP_GATTS_WRITE_EVT:
            ESP_LOGI(TAG, "ESP_GATTS_WRITE_EVT");
            break;

        case ESP_GATTS_CONF_EVT:
            ESP_LOGI(TAG, "ESP_GATTS_CONF_EVT");
            break;

        case ESP_GATTS_UNREG_EVT:
            ESP_LOGI(TAG, "ESP_GATTS_UNREG_EVT");
            break;

        default:
            ESP_LOGI(TAG, "Unhandled GATT server event: %d", event);
            break;
    }
}

void BleWrapper::gap_event_handler(esp_gap_ble_cb_event_t event, esp_ble_gap_cb_param_t *param) {
    // Handle GAP events
    switch (event) {
        case ESP_GAP_BLE_ADV_DATA_SET_COMPLETE_EVT:
            ESP_LOGI(TAG, "ESP_GAP_BLE_ADV_DATA_SET_COMPLETE_EVT");
            esp_ble_gap_start_advertising(ESP_BLE_ADV_PARAMS_DEFAULT);
            break;

        case ESP_GAP_BLE_SCAN_RSP_DATA_SET_COMPLETE_EVT:
            ESP_LOGI(TAG, "ESP_GAP_BLE_SCAN_RSP_DATA_SET_COMPLETE_EVT");
            break;

        case ESP_GAP_BLE_ADV_START_COMPLETE_EVT:
            if (param->adv_start_cmpl.status != ESP_BT_STATUS_SUCCESS) {
                ESP_LOGE(TAG, "Advertising start failed, error code=0x%x", param->scan_start_cmpl.status);
            } else {
                ESP_LOGI(TAG, "Advertising started successfully");
            }
            break;

        case ESP_GAP_BLE_ADV_STOP_COMPLETE_EVT:
            if (param->adv_stop_cmpl.status != ESP_BT_STATUS_SUCCESS) {
                ESP_LOGE(TAG, "Advertising stop failed, error code=0x%x", param->adv_stop_cmpl.status);
            } else {
                ESP_LOGI(TAG, "Advertising stopped successfully");
            }
            break;

        case ESP_GAP_BLE_UPDATE_CONN_PARAMS_EVT:
            ESP_LOGI(TAG, "ESP_GAP_BLE_UPDATE_CONN_PARAMS_EVT");
            break;

        default:
            ESP_LOGI(TAG, "Unhandled GAP event: %d", event);
            break;
    }
}

