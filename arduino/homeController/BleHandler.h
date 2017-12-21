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
    virtual void onReceiveNotify(BTAddr &addr, uint8_t *pData, uint8_t len);
    virtual void onConnected(BTAddr &addr);
    virtual void onWritten(bool success);
    virtual void onDisconnected(BTAddr &addr);
    virtual void onConnectFailed(BTAddr &addr);
    bool connect(BTAddr &addr);
    void disconnect();
    void execute();
    void clear();
  	void addCmd(tBleCmd *cmd);
    bool getCmd(tBleCmd *cmd);
    bool peekCmd(tBleCmd *cmd);
    bool hasCmd();
  
  protected:
    xQueueHandle queue;
};

#endif
