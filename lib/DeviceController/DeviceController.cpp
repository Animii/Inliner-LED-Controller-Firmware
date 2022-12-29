#include "DeviceController.hpp"
#include "esp_log.h"
#include <memory>

DeviceController::DeviceController(AnimationController &animationController, EspNowController &espNowController, InputController &inputController, LIS2DW12Controller &lis2dw12Controller, BLEController &bleController)
{
    _animationController = &animationController;
    _espNowController = &espNowController;
    _inputController = &inputController;
    _lis2dw12Controller = &lis2dw12Controller;
    _bleController = &bleController;

    _deviceSyncPackage.index = _animationController->getCurrentAnimationIndex();
}

DeviceController::~DeviceController()
{
}

void DeviceController::init()
{
    // this->_espNowController->setReceiveCallback(std::bind(&DeviceController::onSyncPackageReceived, this, std::placeholders::_1));
    this->_inputController->configureGPIO(GPIO_NUM_0, std::bind(&DeviceController::onGPIOInput, this));
    this->_lis2dw12Controller->registerSingleTapCallback(std::bind(&DeviceController::onSingleTap, this, std::placeholders::_1));
    this->_lis2dw12Controller->registerDoubleTapCallback(std::bind(&DeviceController::onDoubleTap, this, std::placeholders::_1));

    initBLE();

    this->_animationController->setBrightness(255);
    this->_animationController->notifyBleConfig();

    this->_initialized = true;
}

void DeviceController::onBLEAnimationController(const uint8_t *data, size_t size)
{
    if (size != sizeof(uint32_t))
        return;
    this->_animationController->play(*(uint32_t *)data);
}

void DeviceController::onBLEAnimation(const uint8_t *data, size_t size)
{
}

void DeviceController::onBLELis2dw12(const uint8_t *data, size_t size)
{
}

void DeviceController::initBLE()
{
    // const std::string SERVICE_UUID = "000000ff-0000-1000-8000-00805f9b34fb";
    // const std::string LIS2DW_CHARACTERISTIC_UUID = "0000ff01-0000-1000-8000-00805f9b34fb";
    // const std::string ANIMATION_CONTROLLER_CHARACTERISTIC_UUID = "0000ff02-0000-1000-8000-00805f9b34fb";
    // const std::string ANIMATION_CHARACTERISTIC_UUID = "0000ff03-0000-1000-8000-00805f9b34fb";

    //_animationController->initBLEComponent();

    _bleController->addService("000000ff-0000-1000-8000-00805f9b34fb");
    _bleController->addCharacteristicToService("000000ff-0000-1000-8000-00805f9b34fb", "0000ff0f-0000-1000-8000-00805f9b34fb");
    _bleController->setCharacteristicCallback("0000ff0f-0000-1000-8000-00805f9b34fb", [this](const uint8_t *data, size_t size)
                                              {
    if (size < 1) {
        // Invalid data size, ignore
        return;
    }

    uint8_t offset = data[0];
    size_t numColors = (size - 1) / 3;

    ESP_LOGI("Received Data", "Offset: %d, Num Colors: %d", offset, numColors);

    for (size_t i = 0; i < numColors && (i * 3 + 1) < size; ++i) {
        uint8_t red   = data[i * 3 + 1];
        uint8_t green = data[i * 3 + 2];
        uint8_t blue  = data[i * 3 + 3];

        // Calculate the adjusted pixel index using the received offset
        size_t pixelIndex = i + offset;

        //ESP_LOGI("Set Pixel", "Index: %d, R: %d, G: %d, B: %d", pixelIndex, red, green, blue);
        
        // Set pixel color
        this->_animationController->_ledController.setPixelRGB(pixelIndex, red, green, blue);
       
    }

    this->_animationController->_ledController.refresh(); });
    _bleController->addCharacteristicToService("000000ff-0000-1000-8000-00805f9b34fb", "0000ff04-0000-1000-8000-00805f9b34fb");
    _bleController->setCharacteristicCallback("0000ff04-0000-1000-8000-00805f9b34fb", [this](const uint8_t *data, size_t size)
                                              {

        if(size != 1) return;

        float brightness = data[0] / 255.0f;
       
        // Set pixel color, ensuring `i` is a valid index for your setup
        this->_animationController->_ledController.setBrightness(brightness);
        this->_animationController->_ledController.refresh(); });

    _bleController->startServices();
    _bleController->startAdvertising();
}

void DeviceController::update()
{
    if (!this->_initialized)
        return;

    this->handleSyncPackage();
    this->updateIfMaster();
    this->updateIfSlave();
}

void DeviceController::onSyncPackageReceived(const t_esp_now_sync_package &package)
{
    this->_syncPackageReveived = true;
    this->_receivedSyncPackage = &package;
}

void DeviceController::onGPIOInput()
{
    this->_animationController->next();
    setMaster();
}

void DeviceController::onSingleTap(t_lis2dw12_event_data eventData)
{
}
void DeviceController::onDoubleTap(t_lis2dw12_event_data eventData)
{
    this->_animationController->next();
    setMaster();
}
void onLis2dw12EventData(t_lis2dw12_event_data eventData)
{
}

void DeviceController::handleSyncPackage(void)
{
    if (!this->_syncPackageReveived || this->_receivedSyncPackage == NULL)
        return;

    ESP_LOGI("DEVICE:", "rp %d , sp %d , spt %d, rpt %d , ri %d, si %d", _receivedSyncPackage->isMain, this->_deviceSyncPackage.isMain, this->_deviceSyncPackage.isMainSinceTicks, _receivedSyncPackage->isMainSinceTicks, _receivedSyncPackage->index, this->_animationController->getCurrentAnimationIndex());

    if (_receivedSyncPackage->isMain && this->_deviceSyncPackage.isMain && this->_deviceSyncPackage.isMainSinceTicks >= _receivedSyncPackage->isMainSinceTicks)
    {
        ESP_LOGI("DEVICE:", "Set as Slave");
        setSlave();
    }

    this->_syncPackageReveived = false;
}

void DeviceController::updateIfMaster(void)
{
    if (!_deviceSyncPackage.isMain)
        return;

    this->_animationController->setTimeClock(true);

    this->_deviceSyncPackage.currentTime = xTaskGetTickCount();
    this->_deviceSyncPackage.isMainSinceTicks = xTaskGetTickCount() - this->_isMasterSince;
    this->_deviceSyncPackage.index = this->_animationController->getCurrentAnimationIndex();

    this->_espNowController->sync(this->_deviceSyncPackage);
}

void DeviceController::updateIfSlave(void)
{
    if (_deviceSyncPackage.isMain || this->_receivedSyncPackage == NULL)
        return;

    if (this->_animationController->getCurrentAnimationIndex() != this->_receivedSyncPackage->index)
    {
        this->_animationController->play(this->_receivedSyncPackage->index);
    }

    this->_animationController->forceUpdate(this->_receivedSyncPackage->currentTime);
}

void DeviceController::setMaster()
{
    this->_deviceSyncPackage.isMain = true;
    this->_deviceSyncPackage.isMainSinceTicks = 0;
    this->_isMasterSince = xTaskGetTickCount();
    this->_animationController->setTimeClock(true);
}

void DeviceController::setSlave()
{
    this->_deviceSyncPackage.isMain = false;
    this->_deviceSyncPackage.isMainSinceTicks = 0;
    this->_animationController->setTimeClock(false);
}
