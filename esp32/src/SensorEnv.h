// Copyright 2017 Stefan Schmidt
#ifndef _SENSOR_ENV_H_
#define _SENSOR_ENV_H_

#include "common.h"

class MqttHandler;

class SensorEnv {

public:
    SensorEnv(MqttHandler &m, uint8_t theEnvId, const char *theTopic);
    virtual void init() {}
    virtual bool publish(float temperature, float humidity, float pressure);
    void publishChannelData(const char *channel, float value);
    MqttHandler & mqttHandler;
    uint8_t sensorId;
    String topic;
};

#endif