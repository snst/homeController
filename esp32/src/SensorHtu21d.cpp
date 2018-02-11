// Copyright 2017 Stefan Schmidt
#include "configuration.h"

#include "SensorHtu21d.h"
#include "common.h"
#include "IDeviceI2C.h"
#include "IDataSink.h"

SensorHtu21d::SensorHtu21d(IDeviceI2C *_i2c, IDataSink *_dataSink, uint32_t interval)
    : Executor(interval), i2c(_i2c), dataSink(_dataSink)
{
}

void SensorHtu21d::init()
{
    p(1, "SensorHtu21d::init()\n");
    i2c->begin();
    sensor.begin(i2c);
}

uint32_t SensorHtu21d::execute()
{
    p(1, "SensorHtu21d::execute()\n");
    dataSink->setTemperature(sensor.readTemperature());
    dataSink->setHumidity(sensor.readHumidity());
    bool delay = dataSink->publish() ? 0 : 5000;
    return delay;
}

