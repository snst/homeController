// Copyright 2017 Stefan Schmidt
#include "configuration.h"

#ifdef USE_HTU21D
#include "SensorHtu21d.h"
#include "common.h"
#include <Wire.h>

SensorHtu21d::SensorHtu21d(MqttHandler & m, uint8_t theSensorId, const char *theTopic, int bus)
    : SensorEnv(m, theSensorId, theTopic, bus) {
}

void SensorHtu21d::init(int sda, int scl, uint32_t frequency) 
{
    SensorEnv::init(sda, scl, frequency);
    sensor.begin(wire);
}

bool SensorHtu21d::execute() {

    float temperature = sensor.readTemperature();
    float pressure = 0.0f;
    float humidity = sensor.readHumidity();

    return publish(temperature, humidity, pressure);
}

#endif