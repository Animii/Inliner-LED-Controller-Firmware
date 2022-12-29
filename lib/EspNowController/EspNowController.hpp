#pragma once

#include <cstdint>
#include <esp_now.h>
#include <functional> // Include the functional library for std::function

typedef struct
{
    uint32_t index;
    uint32_t currentTime;
    uint8_t isMain;
    uint32_t isMainSinceTicks;
} t_esp_now_sync_package;

class EspNowController
{
public:
    static EspNowController &getInstance();
    using ReceiveCallback = std::function<void(const t_esp_now_sync_package &package)>;
    void setup();
    void sync(
        t_esp_now_sync_package &syncPackage);
    void sendBroadcast(uint8_t *data, int data_len);
    void setReceiveCallback(const ReceiveCallback &callback);

private:
    EspNowController();
    ~EspNowController();

    void wifiInit();
    void sendCallback(const uint8_t *mac_addr, esp_now_send_status_t status);
    void receiveCallback(const uint8_t *mac_addr, const uint8_t *data, int data_len);

    bool initialized = false;
    ReceiveCallback receiveCallbackFunction; // Store the receive callback function

    // Static callback functions
    static void staticSendCallback(const uint8_t *mac_addr, esp_now_send_status_t status);
    static void staticReceiveCallback(const uint8_t *mac_addr, const uint8_t *data, int data_len);
};