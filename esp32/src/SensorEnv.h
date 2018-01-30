// Copyright 2017 Stefan Schmidt
#ifndef _SENSOR_ENV_H_
#define _SENSOR_ENV_H_

#include "common.h"
#include <Wire.h>

class MqttHandler;

class SensorEnv {

public:
    SensorEnv(MqttHandler &m, uint8_t theEnvId, const char *theTopic, int bus);
    virtual void init(int sda, int scl, uint32_t frequency);
    virtual bool publish(float temperature, float humidity, float pressure);
    void publishChannelData(const char *channel, float value);
    MqttHandler & mqttHandler;
    uint8_t sensorId;
    String topic;
    TwoWire wire;

};

#endif