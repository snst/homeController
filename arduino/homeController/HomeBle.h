// Copyright 2017 Stefan Schmidt

#ifndef _HOME_BLE_H__
#define _HOME_BLE_H__

#include "SimpleEsp32Ble.h"
class BLEAddr;
void setMqttResponseStatus(BLEAddr* addr, uint8_t* pData, size_t length);

class HomeBLE : public SimpleBLE 
{
  protected:
    uint8_t bleCmd[7];
    uint8_t bleCmdLen;
    BLEAddr bleAddr;

  public:
    HomeBLE();
    virtual ~HomeBLE();
    virtual void onReceiveNotify(uint8_t* pData, uint8_t len);
    virtual void onConnected();
    virtual void onWritten(bool success);
    virtual void onDisconnected();
    void execute();
    bool isReady();
    void writeCmd(BLEAddr& addr, uint8_t* cmd, uint8_t cmdLen);
};

#endif
