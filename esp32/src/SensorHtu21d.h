// Copyright 2017 Stefan Schmidt
#ifndef _SENSOR_HTU21D_H_
#define _SENSOR_HTU21D_H_

#include "htu21d.h"
#include "Executor.h"

class IDeviceI2C;
class IDataSink;

class SensorHtu21d : public Executor
{
public:
    SensorHtu21d(IDeviceI2C *_i2c, IDataSink *_dataSink, uint32_t interval);
    void init();
    uint32_t execute();
    HTU21D sensor;
    IDeviceI2C *i2c;
    IDataSink *dataSink;
};

#endif