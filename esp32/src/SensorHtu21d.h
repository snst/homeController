// Copyright 2017 Stefan Schmidt
#ifndef _SENSOR_HTU21D_H_
#define _SENSOR_HTU21D_H_

#include "SensorEnv.h"
//#include "SparkFunHTU21D.h"
//#include "Adafruit_HTU21DF.h"
//#include "HTU21D.h"
#include "htu21d.h"

class SensorHtu21d : public SensorEnv {

public:
    SensorHtu21d(MqttHandler &m, uint8_t theSensorId, const char *theTopic, int bus);
    void init(int sda, int scl, uint32_t frequency);
    bool execute();
    HTU21D sensor;
  //  Adafruit_HTU21DF sensor;
};

#endif