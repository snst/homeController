// Copyright (c) 2017 Adys Tech
// Author: Adarsha (mvadu)
// License : MIT License
// ref : http://www.te.com/commerce/DocumentDelivery/DDEController?Action=srchrtrv&DocNm=HPC199_6&DocType=Data+Sheet&DocLang=English
//       https://github.com/sparkfun/HTU21D_Breakout/blob/master/Libraries/Arduino/src/SparkFunHTU21D.cpp
//       https://github.com/ms-iot/htu21d/blob/develop/HTU21D.cs
// Uses native i2c headers, enable i2c in raspi-config, and reboot.
// modified by Stefan Schmidt

#include <stdio.h>
#include <stdint.h>
#include <Wire.h>
#include <string.h>
#include <errno.h>
#include <time.h>
#include <math.h>
#include "htu21d.h"
#include "common.h"
#include "IDeviceI2C.h"

HTU21D::HTU21D()
: _sensorAddress(HTU21D_Default_Address), wire(NULL)
{
}

bool HTU21D::begin(IDeviceI2C *_i2c)
{
    wire = _i2c;
    resolution = Rh11_Temp11;
    return reset();
}

HTU21D::~HTU21D()
{
}

bool HTU21D::reset()
{
    delay(20);
    wire->beginTransmission(_sensorAddress);
    wire->write(Commands::Reset);
    wire->endTransmission();
    delay(50);
    setResolution(resolution);
    delay(20);
    return true;
}

void HTU21D::setResolution(Resolution res)
{
    uint8_t user = readUserRegister();
    user = (res & 0x81) | (user & 0x7E);
    writeUserRegister(user);
}

float HTU21D::readTemperature()
{
    uint8_t len = 3;
    uint8_t rbuf[len];

    if (readOutput(rbuf, Commands::Measure_Temperature_NoHold, len) != len)
        return NAN;

    // Reconstruct the result using the first two bytes returned from the device
    // NOTE: Zero out the status bits (bits 0 and 1 of the LSB), but keep them in place
    // - status bit 0 - not assigned
    // - status bit 1
    // -- off = temperature data
    // -- on = humdity data
    uint16_t rawValue = (((uint16_t)rbuf[0] << 8) | (uint16_t)rbuf[1]) & 0xFFFC;

    if (ValidCyclicRedundancyCheck(rawValue, rbuf[2]))
        return (rawValue * 175.72 / 65536 - 46.85) + TEMP_OFFSET_HTU21D;
    return NAN;
}


float HTU21D::readHumidity()
{
    uint8_t len = 3;
    uint8_t rbuf[len];

    if (readOutput(rbuf, Commands::Measure_Humidity_NoHold, len) != len)
        return NAN;

    uint16_t rawValue = (((uint16_t)rbuf[0] << 8) | (uint16_t)rbuf[1]) & 0xFFFC;

    //Given the raw humidity data, calculate the actual relative humidity
    if (ValidCyclicRedundancyCheck(rawValue, rbuf[2] ^ 0x62))
        return (rawValue * (125.0 / 65536.0) - 6.0);
    return NAN;
}

uint8_t HTU21D::readUserRegister()
{
    wire->beginTransmission(_sensorAddress);
    wire->write(Commands::Read_User_Register);
    wire->endTransmission();
    wire->requestFrom(_sensorAddress, 1);
    uint8_t user = wire->read();
    return user;
}
 
void HTU21D::writeUserRegister(uint8_t data)
{
    wire->beginTransmission(_sensorAddress);
    wire->write(Commands::Write_User_Register);
    wire->write(data);
    wire->endTransmission();
}

uint8_t HTU21D::readOutput(uint8_t *arrPtr, uint8_t Command, uint8_t len)
{
    uint8_t i = 0;
    uint8_t val = 0;
    wire->beginTransmission(_sensorAddress);
    wire->write(Command);
    wire->endTransmission();

    delay(85);
    // request bytes from slave device
    wire->requestFrom(_sensorAddress, len);
    while (/*(wire->available()) &&*/ (i < len)) // slave may send less than requested
    {
        *arrPtr = wire->read(); // receive a byte
        arrPtr++;
        i++;
    }
    return i;
}


bool HTU21D::ValidCyclicRedundancyCheck(uint16_t data, uint8_t crc)
{
    // Validate the 8-bit cyclic redundancy check (CRC) byte
    // CRC: http://en.wikipedia.org/wiki/Cyclic_redundancy_check
    // Generator polynomial x^8 + x^5 + x^4 + 1: 100110001(0x0131)
    const int CrcBitLength = 8;
    const int DataLength = 16;
    const uint16_t GeneratorPolynomial = 0x0131;

    // input bit first padded with zeroes corresponding to the bit length n of the CRC
    uint32_t crcData = data << CrcBitLength;

    for (int i = DataLength - 1; 0 <= i; --i)
    {
        if (0 == (0x01 & (crcData >> (CrcBitLength + i))))
            continue;
        crcData ^= GeneratorPolynomial << i;
    }

    return (crc == crcData);
}
/*
float HTU21D::getDewPoint()
{
    if (isnan(_dewpoint))
    {
        refreshReading();
    }
    return _dewpoint;
}

float HTU21D::calculateDewPoint()
{
    const float DewConstA = 8.1332;
    const float DewConstB = 1762.39;
    const float DewConstC = 235.66;

    float paritalPressure;
    float dewPoint;

    // To calculate the dew point, the partial pressure must be determined first.
    // See datasheet page 16 for details.
    // Partial pressure = 10 ^ (A - (B / (Temp + C)))
    paritalPressure = DewConstA - (DewConstB / (_temp + DewConstC));
    paritalPressure = pow(10, paritalPressure);

    // Dew point is calculated using the partial pressure, humidity and temperature.
    // The datasheet says "Ambient humidity in %RH, computed from HTU21D(F) sensor" on page 16 is doesn't say to use the temperature compensated
    // RH value. Therefore, we use the raw RH value straight from the sensor.
    // Dew point = -(C + B / (log(RH * PartialPress / 100) - A))
    dewPoint = _humidity * paritalPressure / 100;
    dewPoint = log10(dewPoint) - DewConstA;
    dewPoint = DewConstB / dewPoint;
    dewPoint = -(dewPoint + DewConstC);

    return (dewPoint);
}

float HTU21D::getHeatIndex()
{
    if (isnan(_heatindex))
    {
        refreshReading();
    }
    return _heatindex;
}

//ref: http://www.wpc.ncep.noaa.gov/html/heatindex_equation.shtml
float HTU21D::calculateHeatIndex()
{
    //convert temp to fahrehneit
    float T = 9.0 / 5.0 * _temp + 32;

    //this simple formula is computed first, if it's 80 degrees F or higher, the full regression equation along with any adjustment is applied.
    float heatIndex = 0.5 * (T + 61.0 + ((T - 68.0) * 1.2) + (_humidity * 0.094));

    if (heatIndex > 80)
    {
        float heatIndex = -42.379 + 2.04901523 * T + 10.14333127 * _humidity - .22475541 * T * _humidity - .00683783 * T * T - .05481717 * _humidity * _humidity + .00122874 * T * T * _humidity + .00085282 * T * _humidity * _humidity - .00000199 * T * T * _humidity * _humidity;
        if (_humidity < 13 && T > 80 && T < 112)
        {
            heatIndex -= ((13 - _humidity) / 4) * sqrt((17 - (T > 95.0 ? T - 95.0 : 95.0 - T)) / 17);
        }
        else if (_humidity > 85 && T > 80 && T < 87)
        {
            heatIndex += ((_humidity - 85) / 10) * ((87 - T) / 5);
        }
    }
    //convert back to celsius
    return (5.0 / 9.0 * (heatIndex - 32));
}
*/