// Copyright 2017 Stefan Schmidt
#include "configuration.h"

#include "SensorHtu21d.h"
#include "common.h"
#include "IDeviceI2C.h"
#include "IDataSink.h"

SensorHtu21d::SensorHtu21d(IDeviceI2C *_i2c, IDataSink *_dataSink)
    : i2c(_i2c), dataSink(_dataSink)
{
}

void SensorHtu21d::begin()
{
    i2c->begin();
    sensor.begin(i2c);
}

bool SensorHtu21d::execute()
{
    dataSink->setTemperature(sensor.readTemperature());
    dataSink->setHumidity(sensor.readHumidity());
    return dataSink->publish();
}

