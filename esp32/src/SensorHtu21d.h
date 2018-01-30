// Copyright 2017 Stefan Schmidt
#ifndef _SENSOR_HTU21D_H_
#define _SENSOR_HTU21D_H_

#include "SensorEnv.h"
#include "htu21d.h"

class SensorHtu21d : public SensorEnv {

public:
    SensorHtu21d(uint8_t bus, uint8_t _sda, uint8_t _scl, uint32_t _frequency, MqttHandler &m, uint8_t _sensorId, const char *_topic);
    void begin();
    bool execute();
    HTU21D sensor;
};

#endif