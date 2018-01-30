// Copyright 2017 Stefan Schmidt
#ifndef _SENSOR_ENV_H_
#define _SENSOR_ENV_H_

#include "common.h"
#include <Wire.h>

class MqttHandler;

class SensorEnv {

public:
    SensorEnv(uint8_t bus, uint8_t _sda, uint8_t _scl, uint32_t _frequency, MqttHandler &m, uint8_t _sensorId, const char *_topic);
    virtual void begin();
    virtual bool publish(float temperature, float humidity, float pressure);
    void publishChannelData(const char *channel, float value);
    MqttHandler & mqttHandler;
    uint8_t sensorId;
    String topic;
    TwoWire wire;
    uint8_t sda;
    uint8_t scl;
    uint32_t frequency;
};

#endif