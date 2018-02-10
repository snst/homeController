// Copyright 2017 Stefan Schmidt
#include "configuration.h"

#include "SensorBme280.h"
#include "common.h"
#include "IDeviceI2C.h"
#include "IDataSink.h"

SensorBme280::SensorBme280(IDeviceI2C *_i2c, IDataSink *_dataSink)
    : i2c(_i2c), dataSink(_dataSink)
{
}

void SensorBme280::begin() 
{
    i2c->begin();
    sensor.begin(i2c);
}

bool SensorBme280::execute() 
{
    bool ret = sensor.takeForcedMeasurement();
    if (ret) {
        dataSink->setTemperature(sensor.readTemperature());
        dataSink->setPressure(sensor.readPressure() / 100.0f);
        dataSink->setHumidity(sensor.readHumidity());
        ret = dataSink->publish();
        if (!ret) {
            p(1, "reset BME280\n");
//            sleep(50);
//            sensor.init();
        }

    } else {
        p(1, "BME280: forced timeout\n");
    }
    return ret;
}
