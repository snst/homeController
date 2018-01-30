// Copyright 2017 Stefan Schmidt
#include "configuration.h"

#include "SensorBme280.h"
#include "common.h"

SensorBme280::SensorBme280(uint8_t bus, uint8_t _sda, uint8_t _scl, uint32_t _frequency, MqttHandler &m, uint8_t _sensorId, const char *_topic)
    : SensorEnv(bus, _sda, _scl, _frequency, m, _sensorId, _topic) 
{
}

void SensorBme280::begin() 
{
    SensorEnv::begin();
    sensor.begin(&wire);
}

bool SensorBme280::execute() 
{
    float temperature = sensor.readTemperature();
    float pressure = sensor.readPressure() / 100.0f;
    float humidity = sensor.readHumidity();
    return publish(temperature, humidity, pressure);
}
