#include "AnimationController.hpp"
#include "EspNowController.hpp"
#include "InputController.hpp" 
#include "LIS2DW12Controller.hpp"
#include "BLEController.hpp"

class DeviceController
{
private:
    
    EspNowController *_espNowController;
    InputController* _inputController;
    LIS2DW12Controller*  _lis2dw12Controller;
    BLEController* _bleController;

    bool _isMaster = true;
    bool _initialized = false;
    bool _syncPackageReveived = false;
    uint32_t _isMasterSince = 0;

    const t_esp_now_sync_package *_receivedSyncPackage;
    t_esp_now_sync_package _deviceSyncPackage = {
        .index = 0,
        .currentTime = 0,
        .isMain = true,
        .isMainSinceTicks = 0,
    };

    void onSyncPackageReceived(const t_esp_now_sync_package &package);
    void onGPIOInput();
    void onSingleTap(t_lis2dw12_event_data eventData);
    void onDoubleTap(t_lis2dw12_event_data eventData);
    void onLis2dw12EventData(t_lis2dw12_event_data eventData);

    void handleSyncPackage(void);

    void updateIfMaster(void);
    void updateIfSlave(void);

    void setMaster();
    void setSlave();


    void initBLE();
    void onBLEAnimationController(const uint8_t* data, size_t size);
    void onBLEAnimation(const uint8_t* data, size_t size);
    void onBLELis2dw12(const uint8_t* data, size_t size);

public:
    DeviceController(AnimationController &animationController, EspNowController &espNowController, InputController &inputController,LIS2DW12Controller &lis2dw12Controller,BLEController &bleController);
    ~DeviceController();
    AnimationController *_animationController;
    void init(void);
    void update(void);
};