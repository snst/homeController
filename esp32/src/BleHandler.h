// Copyright 2017 Stefan Schmidt

#ifndef _HOME_BLE_H__
#define _HOME_BLE_H__

#include "BleBase.h"
#include "common.h"
#include "BTAddr.h"


class BleHandler : public BleBase 
{
  public:
    BleHandler();
    virtual ~BleHandler();
    virtual void onReceiveData(const BTAddr &addr, const uint8_t *pData, uint8_t len);
    virtual void onConnected(const BTAddr &addr);
    virtual void onWritten(bool success);
    virtual void onDisconnected(const BTAddr &addr);
    virtual void onConnectFailed(const BTAddr &addr);
    bool connect(const BTAddr &addr);
    void disconnect();
    void execute();
    void clear();
  	void addCmd(const tBleCmd &cmd);
    bool getCmd(tBleCmd &cmd);
    bool hasCmd();
  
  protected:
    xQueueHandle queue;
    SemaphoreHandle_t queueMutex;
};

#endif
