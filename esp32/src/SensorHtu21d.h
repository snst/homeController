// Copyright 2017 Stefan Schmidt
#ifndef _SENSOR_HTU21D_H_
#define _SENSOR_HTU21D_H_

#include "htu21d.h"

class IDeviceI2C;
class IDataSink;

class SensorHtu21d
{
public:
    SensorHtu21d(IDeviceI2C *_i2c, IDataSink *_dataSink);
    void begin();
    bool execute();
    HTU21D sensor;
    IDeviceI2C *i2c;
    IDataSink *dataSink;
};

#endif