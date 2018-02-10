// Copyright 2017 Stefan Schmidt

#ifndef _DEVICEI2CHARD_H_
#define _DEVICEI2CHARD_H_

#include <Arduino.h>
#include "IDeviceI2C.h"
#include "Wire.h"

class DeviceI2CHard : public IDeviceI2C
{
  public:
    DeviceI2CHard(uint8_t _scl, uint8_t _sda);
    DeviceI2CHard(uint8_t _bus, uint8_t _scl, uint8_t _sda, uint32_t _frequency);
    virtual ~DeviceI2CHard();
    void beginTransmission(uint8_t addr);
    virtual uint8_t write(uint8_t val);
    virtual void endTransmission();
    virtual void requestFrom(uint8_t addr, uint8_t len);
    virtual uint8_t read();
    virtual uint8_t readLast();    
    void begin();

  protected:
    TwoWire wire;
    uint8_t scl;
    uint8_t sda;
    uint32_t frequency;
};

#endif
