// Copyright 2017 Stefan Schmidt
#include "configuration.h"

#include "SensorBme280.h"
#include "common.h"
#include "IDeviceI2C.h"
#include "IDataSink.h"

SensorBme280::SensorBme280(IDeviceI2C *_i2c, IDataSink *_dataSink, uint32_t interval)
    : Executor(interval), i2c(_i2c), dataSink(_dataSink), isWorking(false)
{
}

void SensorBme280::init() 
{
    p(1, "SensorBme280::init()\n");
    i2c->begin();
    sensor.begin(i2c);
}

uint32_t SensorBme280::execute() 
{
    uint32_t delay = 0;
    p(1, "SensorBme280::execute()\n");
    if (!isWorking) {
        isWorking = true;
        p(1, "SensorBme280::takeForcedMeasurement()\n");
        sensor.takeForcedMeasurement();
        delay = 2000;
    } else {
        bool res = sensor.isMeasuementReady();
        p(1, "SensorBme280::readTemperature(), ready=%d\n", res);
        if (res) {
            dataSink->setTemperature(sensor.readTemperature());
            dataSink->setPressure(sensor.readPressure() / 100.0f);
            dataSink->setHumidity(sensor.readHumidity());
            res = dataSink->publish();
            isWorking = false;
        } else {
            delay = 1000;
        }
    }
    return delay;
}
