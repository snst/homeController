// Copyright 2017 Stefan Schmidt
#ifndef _SENSOR_BME280_H_
#define _SENSOR_BME280_H_

#include "Adafruit_BME280.h"

class IDeviceI2C;
class IDataSink;

class SensorBme280
{
public:
    SensorBme280(IDeviceI2C *_i2c, IDataSink *_dataSink);
    void begin();
    bool execute();
    Adafruit_BME280 sensor;
    IDeviceI2C *i2c;
    IDataSink *dataSink;
};

#endif