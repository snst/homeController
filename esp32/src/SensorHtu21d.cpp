// Copyright 2017 Stefan Schmidt
#include "configuration.h"

#include "SensorHtu21d.h"
#include "common.h"
#include <Wire.h>

SensorHtu21d::SensorHtu21d(uint8_t bus, uint8_t _sda, uint8_t _scl, uint32_t _frequency, MqttHandler &m, uint8_t _sensorId, const char *_topic)
    : SensorEnv(bus, _sda, _scl, _frequency, m, _sensorId, _topic) 
{
}

void SensorHtu21d::begin()
{
    SensorEnv::begin();
    sensor.begin(wire);
}

bool SensorHtu21d::execute()
{
    float temperature = sensor.readTemperature();
    float pressure = 0.0f;
    float humidity = sensor.readHumidity();
    return publish(temperature, humidity, pressure);
}

