// Copyright 2017 Stefan Schmidt
#ifndef _DATASINK_H_
#define _DATASINK_H_

#include "common.h"
#include <Wire.h>
#include "IDataSink.h"

class MqttHandler;

class DataSink : public IDataSink 
{
public:
    DataSink(MqttHandler &_mqtt, uint8_t _sensorId, const char *_topic);
    virtual void setTemperature(float val);
    virtual void setPressure(float val);
    virtual void setHumidity(float val);
    virtual bool publish();
    void publishChannelData(const char *channel, float value);
    void reset();
    MqttHandler &mqtt;
    uint8_t sensorId;
    String topic;
    float temperature;
    float pressure;
    float humidity;
};

#endif