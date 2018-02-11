// Copyright 2017 Stefan Schmidt
#ifndef _DEVICEI2CSOFT_H_
#define _DEVICEI2CSOFT_H_


#include "IDeviceI2C.h"

class DeviceI2CSoft : public IDeviceI2C
{
  public:
    DeviceI2CSoft(uint8_t _scl, uint8_t _sda);
    virtual ~DeviceI2CSoft();
    virtual void beginTransmission(uint8_t addr);
    virtual uint8_t write(uint8_t val);
    virtual void endTransmission();
    virtual void requestFrom(uint8_t addr, uint8_t len);
    virtual uint8_t read();
    virtual uint8_t readLast();    
    void begin();

  protected:
    uint8_t read(bool ack);
    void writeBit(bool high);
    uint8_t readBit();
    void delayBit();
    void delayHalfBit();
    void sdaHigh();
    void sdaLow();
    void sclHigh();
    void sclLow();
    void startBit();
    void stopBit();
    void repeatStartBit();
    void sclStretch();

    uint8_t scl;
    uint8_t sda;
};
#endif
