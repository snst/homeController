// Copyright 2017 Stefan Schmidt
#include "configuration.h"

#ifdef USE_BME280
#include "SensorBme280.h"
#include "common.h"

SensorBme280::SensorBme280(MqttHandler & m, uint8_t theSensorId, const char *theTopic)
    : SensorEnv(m, theSensorId, theTopic) {
}

void SensorBme280::init() {
  sensor.begin();
}

bool SensorBme280::execute() {

    float temperature = sensor.readTemperature();
    float pressure = sensor.readPressure() / 100.0f;
    float humidity = sensor.readHumidity();

    return publish(temperature, humidity, pressure);
}
#endif