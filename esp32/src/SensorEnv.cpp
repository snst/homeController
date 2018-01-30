// Copyright 2017 Stefan Schmidt
#include "configuration.h"

#include "SensorEnv.h"
#include "MqttHandler.h"
#include "common.h"


SensorEnv::SensorEnv(uint8_t bus, uint8_t _sda, uint8_t _scl, uint32_t _frequency, MqttHandler &m, uint8_t _sensorId, const char *_topic)
    : sda(_sda), scl(_scl), frequency(_frequency), mqttHandler(m), sensorId(_sensorId), topic(_topic), wire(bus)
{
}

void SensorEnv::publishChannelData(const char *channel, float value)
{
    char bufTopic[20];
    char buffer[128];
    sprintf(bufTopic, "%s/%s", topic.c_str(), channel);
    sprintf(buffer, "{\"data\": %f, \"write\": true}", value);
    mqttHandler.publish(bufTopic, buffer, strlen(buffer));
}

bool SensorEnv::publish(float temperature, float humidity, float pressure)
{
    p(5, "\nSensor: %s\n", topic.c_str());
    p(5, "Temperature: %fC\n", temperature);
    p(5, "Humidity: %f%%\n", humidity);
    p(5, "Pressure: %fhPa\n", pressure);

    if (temperature > 60.0f || temperature < -40.0f || humidity < 0.0f || humidity > 100.0f) {
        p(5, "Skip invalid values.");
        return false;
    }

    mqttHandler.sendResponseSensor(sensorId, temperature, humidity, pressure);
    publishChannelData("te", temperature);
    publishChannelData("hu", humidity);
    publishChannelData("pr", pressure);

    return true;
}

void SensorEnv::begin() 
{
    wire.begin(sda, scl, frequency);
    wire.setTimeout(500);
}
