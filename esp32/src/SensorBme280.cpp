// Copyright 2017 Stefan Schmidt
#include "configuration.h"

#ifdef USE_BME280
#include "SensorBme280.h"
#include "common.h"

SensorBme280::SensorBme280(MqttHandler & m, uint8_t theSensorId, const char *theTopic, int bus)
    : SensorEnv(m, theSensorId, theTopic, bus) {
}

void SensorBme280::init(int sda, int scl, uint32_t frequency) 
{
    SensorEnv::init(sda, scl, frequency);
    sensor.begin(&wire);
}

bool SensorBme280::execute() {

    float temperature = sensor.readTemperature();
    float pressure = sensor.readPressure() / 100.0f;
    float humidity = sensor.readHumidity();

    return publish(temperature, humidity, pressure);
}
#endif