#include <Arduino.h>
#include "DeviceI2CHard.h"

DeviceI2CHard::DeviceI2CHard(uint8_t _scl, uint8_t _sda)
: wire(0), scl(_scl), sda(_sda), frequency(100000)
{
}

DeviceI2CHard::DeviceI2CHard(uint8_t _bus, uint8_t _scl, uint8_t _sda, uint32_t _frequency)
: wire(_bus), scl(_scl), sda(_sda), frequency(_frequency)
{
}

DeviceI2CHard::~DeviceI2CHard()
{
}

void DeviceI2CHard::begin()
{
    wire.begin(sda, scl, frequency);
    wire.setTimeout(500);
}

void DeviceI2CHard::beginTransmission(uint8_t addr)
{
    wire.beginTransmission(addr);
}

uint8_t DeviceI2CHard::write(uint8_t val)
{
    return wire.write(val);
}

void DeviceI2CHard::endTransmission()
{
    wire.endTransmission();
}

void DeviceI2CHard::requestFrom(uint8_t addr, uint8_t len)
{
    wire.requestFrom(addr, len);
}

uint8_t DeviceI2CHard::read()
{
    return wire.read();
}

uint8_t DeviceI2CHard::readLast()
{
    return wire.read();
}
