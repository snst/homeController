// Copyright 2017 Stefan Schmidt

#ifndef _HOME_BLE_H__
#define _HOME_BLE_H__

#include "SimpleEsp32Ble.h"

void setMqttResponse(uint8_t* pData, size_t length);

class HomeBLE : public SimpleBLE 
{
  public:
    HomeBLE();
    virtual ~HomeBLE();
    virtual void onReceiveNotify(uint8_t* pData, uint8_t len);
    virtual void onConnected();
};

#endif
