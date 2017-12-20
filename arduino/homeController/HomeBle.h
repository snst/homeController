// Copyright 2017 Stefan Schmidt

#ifndef _HOME_BLE_H__
#define _HOME_BLE_H__

#include "SimpleEsp32Ble.h"
#include "common.h"
#include "BTAddr.h"


class HomeBLE : public SimpleBLE 
{
  protected:
    uint8_t bleCmd[7];
    uint8_t bleCmdLen;
    BTAddr bleAddr;

  public:
    HomeBLE();
    virtual ~HomeBLE();
    virtual void onReceiveNotify(uint8_t* pData, uint8_t len);
    virtual void onConnected();
    virtual void onWritten(bool success);
    virtual void onDisconnected();
    virtual void onConnectFailed();
    bool connect(BTAddr& addr);
    void disconnect();
    void execute();
    bool isReady();
    void writeCmd(BTAddr& addr, uint8_t* cmd, uint8_t cmdLen);
};

#endif
