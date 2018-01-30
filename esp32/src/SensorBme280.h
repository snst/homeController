// Copyright 2017 Stefan Schmidt
#ifndef _SENSOR_BME280_H_
#define _SENSOR_BME280_H_

#include "SensorEnv.h"
#include "Adafruit_BME280.h"

class SensorBme280 : public SensorEnv {

public:
    SensorBme280(uint8_t bus, uint8_t _sda, uint8_t _scl, uint32_t _frequency, MqttHandler &m, uint8_t _sensorId, const char *_topic);
    void begin();
    bool execute();
    Adafruit_BME280 sensor;
};

#endif