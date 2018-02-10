// Copyright 2017 Stefan Schmidt
#ifndef _IDEVICE_I2C_H_
#define _IDEVICE_I2C_H_
#include <Arduino.h>

class IDeviceI2C
{
    public:
    virtual void begin() = 0;
    virtual void beginTransmission(uint8_t addr) = 0;
    virtual uint8_t write(uint8_t val) = 0;
    virtual void endTransmission() = 0;
    virtual void requestFrom(uint8_t addr, uint8_t len) = 0;
    virtual uint8_t read() = 0;
    virtual uint8_t readLast() = 0;    
};

#endif