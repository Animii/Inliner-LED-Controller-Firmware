#pragma once
#include "BLEController.hpp"
#include  <memory>
#include <string>

class BLEComponent {
    public:
    BLEComponent(BLEController &bleController,std::string &serviceUUID,std::map<std::string, CharacteristicCallback> &characteristics);
    void initBLEComponent();
    private:
    BLEController *bleController;
    std::string *serviceUUID;
    std::map<std::string, CharacteristicCallback> *characteristics;

    protected:
    void notify(std::string characteristicUUID,uint8_t * data, size_t size);
};