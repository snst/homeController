// Copyright 2017 Stefan Schmidt
#ifndef _SENSOR_HTU21D_H_
#define _SENSOR_HTU21D_H_

#include "SensorEnv.h"
#include "SparkFunHTU21D.h"

class SensorHtu21d : public SensorEnv {

public:
    SensorHtu21d(MqttHandler &m, uint8_t theSensorId, const char *theTopic);
    void init();
    bool execute();
    HTU21D sensor;
};

#endif