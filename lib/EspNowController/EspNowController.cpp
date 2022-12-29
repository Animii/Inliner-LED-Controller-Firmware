// EspNowController.cpp
#include "EspNowController.hpp"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "esp_wifi.h"
#include "esp_netif.h"
#include "esp_now.h"
#include <string.h>

static const char *TAG = "EspNowController";
static const int ESPNOW_CHANNEL = 1;
#if CONFIG_ESPNOW_WIFI_MODE_STATION
#define ESPNOW_WIFI_MODE WIFI_MODE_STA
#define ESPNOW_WIFI_IF ESP_IF_WIFI_STA
#else
#define ESPNOW_WIFI_MODE WIFI_MODE_AP
#define ESPNOW_WIFI_IF (wifi_interface_t) ESP_IF_WIFI_AP
#endif
#define delay_ms(ms) vTaskDelay((ms) / portTICK_RATE_MS)
static constexpr uint8_t broadcastMac[ESP_NOW_ETH_ALEN] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
EspNowController::EspNowController() {
}

EspNowController::~EspNowController() {
    // Clean up resources if needed
}

EspNowController& EspNowController::getInstance() {
    static EspNowController instance;
    return instance;
}

void EspNowController::setup() {
    if (initialized) {
        return;
    }

    esp_log_level_set("*", ESP_LOG_INFO);
    wifiInit();
    esp_now_init();
    esp_now_register_send_cb(staticSendCallback);
    esp_now_register_recv_cb(staticReceiveCallback);

    esp_now_peer_info_t *peer = (esp_now_peer_info_t *)malloc(sizeof(esp_now_peer_info_t));
    if (peer == nullptr) {
        ESP_LOGE(TAG, "Malloc peer information fail");
        return;
    }

    peer->channel = ESPNOW_CHANNEL;
    peer->ifidx = ESPNOW_WIFI_IF;
    peer->encrypt = false;
    memcpy(peer->peer_addr, broadcastMac, ESP_NOW_ETH_ALEN);
    ESP_ERROR_CHECK(esp_now_add_peer(peer));
    free(peer);

    initialized = true;
}

void EspNowController::wifiInit() {
    ESP_ERROR_CHECK(esp_netif_init());
    ESP_ERROR_CHECK(esp_netif_init());
    ESP_ERROR_CHECK(esp_event_loop_create_default());
    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));
    ESP_ERROR_CHECK(esp_wifi_set_storage(WIFI_STORAGE_RAM));
    ESP_ERROR_CHECK(esp_wifi_set_mode(ESPNOW_WIFI_MODE));
    ESP_ERROR_CHECK(esp_wifi_start());
}

void EspNowController::sendCallback(const uint8_t *mac_addr, esp_now_send_status_t status) {
    // Handle send status if needed
}

void EspNowController::receiveCallback(const uint8_t *mac_addr, const uint8_t *data, int data_len) {
    //ESP_LOGI(TAG, "Received data from %02x:%02x:%02x:%02x:%02x:%02x", MAC2STR(mac_addr));

    if (data_len != sizeof(t_esp_now_sync_package)) {
        ESP_LOGE(TAG, "Received data length does not match the expected size.");
        return;
    }

    t_esp_now_sync_package *receivedPackage = (t_esp_now_sync_package *)data;

     // Check if a receive callback has been set and call it with the received package
    if (receiveCallbackFunction) {
        receiveCallbackFunction(*receivedPackage);
    }
    
    delay_ms(5);
}

void EspNowController::setReceiveCallback(const ReceiveCallback& callback ) {
    receiveCallbackFunction = callback;
}

void EspNowController::sync(
    t_esp_now_sync_package &syncPackage) {
    sendBroadcast((uint8_t *)&syncPackage, sizeof(syncPackage));
}

void EspNowController::sendBroadcast(uint8_t *data, int data_len) {
    esp_now_send(broadcastMac, data, data_len);
    // Handle send status if needed
}

// Static callback functions
void EspNowController::staticSendCallback(const uint8_t *mac_addr, esp_now_send_status_t status) {
    getInstance().sendCallback(mac_addr, status);
}

void EspNowController::staticReceiveCallback(const uint8_t *mac_addr, const uint8_t *data, int data_len) {
    getInstance().receiveCallback(mac_addr, data, data_len);
}