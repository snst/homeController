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
    void execute();
    void clear();
  	void addCmd(const tBleCmd &cmd);

  protected:
    virtual void onReceiveData(const BTAddr &addr, const uint8_t *pData, uint8_t len);
    virtual void onConnected(const BTAddr &addr, uint16_t connId);
    virtual void onDisconnected(const BTAddr &addr);
    virtual void onConnectFailed(const BTAddr &addr);
    virtual void setConnState(const BTAddr &addr, eState state, uint16_t connId);
    void disconnect();
    bool getCmd(tBleCmd &cmd);
    void clearCmdFromQueue(const BTAddr &addr);
  
    xQueueHandle queue;
    SemaphoreHandle_t queueMutex;
};

#endif
