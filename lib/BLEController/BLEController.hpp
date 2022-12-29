#pragma once

#include "NimBLEDevice.h"
#include  <memory>

typedef std::function<void(const uint8_t*, size_t)> CharacteristicCallback;

class BLEController : public NimBLECharacteristicCallbacks
{
public:
    
    BLEController();
    ~BLEController();
    void init();
    void setCharacteristicValue(const std::string &characteristicUUID, const uint8_t *data, size_t size);
    void setCharacteristicCallback(const std::string &characteristicUUID, const CharacteristicCallback& callback);

    void addService(const std::string &uuid);
    void startServices(void);
    void addCharacteristicToService(const std::string &serviceUUID, const std::string &characteristicUUID);

    void onWrite(NimBLECharacteristic *pCharacteristic, NimBLEConnInfo &connInfo);

    void startAdvertising(void);

private:
    NimBLEServer *pServer;
    std::map<std::string, NimBLECharacteristic *> characteristics;
    std::map<std::string, NimBLEService *> services;
    std::map<std::string, CharacteristicCallback> characteristicsCallbacks;
};
