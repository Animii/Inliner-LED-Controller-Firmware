#include "BLEComponent.hpp"

BLEComponent::BLEComponent(BLEController &bleController, std::string &serviceUUID, std::map<std::string, CharacteristicCallback> &characteristics)
{
    this->bleController = &bleController;
    this->serviceUUID = &serviceUUID;
    this->characteristics = &characteristics;
}

void BLEComponent::initBLEComponent()
{
    this->bleController->addService(*serviceUUID);
    for (const auto &kv : *characteristics)
    {
       this->bleController->addCharacteristicToService(*serviceUUID,kv.first);
       this->bleController->setCharacteristicCallback(kv.first,kv.second);
    }
    
}

void BLEComponent::notify(std::string characteristicUUID, uint8_t *data, size_t size)
{
    this->bleController->setCharacteristicValue(characteristicUUID,data,size);
}
