// Copyright 2017 Stefan Schmidt
#ifndef _SENSOR_BME280_H_
#define _SENSOR_BME280_H_

#include "SensorEnv.h"
#include "Adafruit_BME280.h"

class SensorBme280 : public SensorEnv {

public:
    SensorBme280(MqttHandler &m, uint8_t theSensorId, const char *theTopic);
    void init();
    bool execute();
    Adafruit_BME280 sensor;
};

#endif