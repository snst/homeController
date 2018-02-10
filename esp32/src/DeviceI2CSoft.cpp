#include <Arduino.h>
#include "DeviceI2CSoft.h"

#define BITDELAY 50

DeviceI2CSoft::DeviceI2CSoft(uint8_t _scl, uint8_t _sda)
: scl(_scl), sda(_sda)
{
}

DeviceI2CSoft::~DeviceI2CSoft()
{
}

void DeviceI2CSoft::begin()
{
    pinMode(scl, OUTPUT);    
    pinMode(sda, OUTPUT);    
    sdaHigh();
    sclHigh();
    delayBit();
}

void DeviceI2CSoft::beginTransmission(uint8_t addr)
{
    startBit();
    write(addr << 1); // no read bit
}

uint8_t DeviceI2CSoft::write(uint8_t val)
{
    for (uint8_t i = 0; i < 8; i++) {
        writeBit( val & 128 );
        val <<= 1;
    }

    return readBit();
}

uint8_t DeviceI2CSoft::readBit()
{
    sdaHigh();
    delayBit();
    sclHigh();
    sclStretch();
    delayBit();
    uint8_t bit = digitalRead(sda);
    sclLow();
    return bit;
}

void DeviceI2CSoft::sclStretch() 
{
    int n=100;
    do {
        delayMicroseconds(BITDELAY/10);
    } while( !digitalRead(scl) && n--);
}

void DeviceI2CSoft::endTransmission()
{
    stopBit();
}

void DeviceI2CSoft::requestFrom(uint8_t addr, uint8_t len)
{
    startBit();
    write((addr << 1) | 1); // set read bit
}

uint8_t DeviceI2CSoft::read()
{
    return read(true);
}

uint8_t DeviceI2CSoft::readLast()
{
    return read(false);
}

void DeviceI2CSoft::writeBit(bool high)
{
    if (high)
        sdaHigh();
    else
        sdaLow();

    delayBit();
    sclHigh();
    delayBit();
    sclLow();
}

void DeviceI2CSoft::sdaHigh()
{
    digitalWrite(sda, HIGH);
}

void DeviceI2CSoft::sdaLow()
{
    digitalWrite(sda, LOW);
}

void DeviceI2CSoft::sclHigh()
{
    digitalWrite(scl, HIGH);
}

void DeviceI2CSoft::sclLow()
{
    digitalWrite(scl, LOW);
}

void DeviceI2CSoft::delayBit()
{
    delayMicroseconds(BITDELAY);
}

void DeviceI2CSoft::delayHalfBit()
{
    delayMicroseconds(BITDELAY/2);
}


uint8_t DeviceI2CSoft::read(bool ack)
{
    uint8_t res = 0;

    for (uint8_t i = 0; i < 8; i++) {
        res <<= 1;
        res |= readBit();  
    }

    writeBit(!ack);

    delayBit();

    return res;
}

void DeviceI2CSoft::startBit()
{
    sdaHigh();
    sclHigh();
    delayHalfBit();
    sdaLow();
    delayHalfBit();
    sclLow();
}

void DeviceI2CSoft::stopBit()
{
    sclHigh();
    delayHalfBit();
    sdaHigh();
    delayHalfBit();
}

void DeviceI2CSoft::repeatStartBit()
{
    /*
    // set both to high at the same time (releases drive on both lines)
    i2c_sda_hi();
    i2c_scl_hi();

    i2c_scl_lo();                           // force SCL low
    _delay_us(i2cbitdelay);

    i2c_sda_release();                      // release SDA
    _delay_us(i2cbitdelay);

    i2c_scl_release();                      // release SCL
    _delay_us(i2cbitdelay);

    i2c_sda_lo();                           // force SDA low
    _delay_us(i2cbitdelay);
    */
}





