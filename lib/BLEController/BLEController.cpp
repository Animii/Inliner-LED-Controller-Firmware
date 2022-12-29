#include "BLEController.hpp"
#include <memory>
static const char *TAG = "BLEController";
BLEController::BLEController()
{
}

BLEController::~BLEController()
{
    // Cleanup NimBLE resources
    // delete pServer;
}

void BLEController::init()
{
    // Initialize NimBLE
    BLEDevice::init("Long name works now");
    pServer = BLEDevice::createServer();

    printf("BLE server initialized!\n");
}

void BLEController::setCharacteristicValue(const std::string &characteristicUUID, const uint8_t *data, size_t size)
{
    auto it = characteristics.find(characteristicUUID);
    if (it != characteristics.end())
    {
        it->second->setValue(data, size);
        it->second->notify(true);
    }
}

void BLEController::setCharacteristicCallback(const std::string &characteristicUUID, const CharacteristicCallback& callback)
{
    auto it = characteristics.find(characteristicUUID);
    if (it != characteristics.end())
    {
        characteristicsCallbacks[characteristicUUID] = callback;
    }
}

void BLEController::addService(const std::string &uuid)
{
    // Create a new service
    services[uuid] = pServer->createService(uuid);
    ESP_LOGI(TAG, "Add Service: %s", uuid.c_str());
}
void BLEController::startServices(void)
{
    for (const auto &kv : services)
    {
        bool status = kv.second->start();
        ESP_LOGI(TAG, "Start Service: %s status: %d", kv.second->getUUID().toString().c_str(), status);
    }
}

void BLEController::addCharacteristicToService(const std::string &serviceUUID, const std::string &characteristicUUID)
{
    auto characteristic = pServer->getServiceByUUID(serviceUUID)->createCharacteristic(characteristicUUID, NIMBLE_PROPERTY::READ | NIMBLE_PROPERTY::WRITE | NIMBLE_PROPERTY::NOTIFY,5024u);
    characteristic->setCallbacks(this);
    characteristics[characteristicUUID] = characteristic;
}

void BLEController::onWrite(NimBLECharacteristic *pCharacteristic, NimBLEConnInfo &connInfo)
{
    //ESP_LOGI(TAG, "onWrite: %s ", pCharacteristic->getUUID().toString().c_str());
    std::string characteristicUUID = pCharacteristic->getUUID().toString();
    auto callbackIt = characteristicsCallbacks.find(characteristicUUID);

    if (callbackIt != characteristicsCallbacks.end() && callbackIt->second != nullptr)
    {
        uint16_t size = pCharacteristic->getValue().size();
        uint8_t buffer[size];
        memcpy(buffer,pCharacteristic->getValue().data(),pCharacteristic->getValue().size());
         
        //ESP_LOGI(TAG, "Size %i ",size);
        //for (size_t i = 0; i < size; i++)
        //{
        //    printf("%i ",buffer[i]);
        //}
        
        // Check if the callback function is valid before calling it
        if(callbackIt->second)
        {
        (callbackIt->second)(buffer, size);
        }
    }
    else
    {
        ESP_LOGE(TAG, "Callback function not found or is a null pointer.");
    }
}

void BLEController::startAdvertising(void)
{
    BLEAdvertising *pAdvertising = BLEDevice::getAdvertising();

    for (const auto &kv : services)
    {
        pAdvertising->addServiceUUID(kv.second->getUUID());
        ESP_LOGI(TAG, "StartAdvertising Service: %s", kv.second->getUUID().toString().c_str());
    }

    pAdvertising->setScanResponse(true);
    BLEDevice::startAdvertising();
}