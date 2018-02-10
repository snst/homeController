// Copyright 2017 Stefan Schmidt
#include "configuration.h"
#include "common.h"
#include "DataSink.h"
#include "MqttHandler.h"


DataSink::DataSink(MqttHandler &_mqtt, uint8_t _sensorId, const char *_topic)
    : mqtt(_mqtt), sensorId(_sensorId), topic(_topic)
{
    reset();
}

void DataSink::reset()
{
    temperature = 0.0f;
    pressure = 0.0f;
    humidity = 0.0f;
}


void DataSink::setTemperature(float val) 
{
    temperature = val;
}

void DataSink::setPressure(float val) 
{
    pressure = val;
}

void DataSink::setHumidity(float val) 
{
    humidity = val;
}

bool DataSink::publish() 
{
    p(5, "\nSensor: %s\n", topic.c_str());
    p(5, "Temperature: %fC\n", temperature);
    p(5, "Humidity: %f%%\n", humidity);
    p(5, "Pressure: %fhPa\n", pressure);

    if (isnan(temperature) || temperature > 60.0f || temperature < -40.0f || humidity < 0.0f || humidity > 100.0f) {
        p(5, "Skip invalid values.");
        return false;
    }

    mqtt.sendResponseSensor(sensorId, temperature, humidity, pressure);
    publishChannelData("te", temperature);
    publishChannelData("hu", humidity);
    publishChannelData("pr", pressure);

    return true;
}


void DataSink::publishChannelData(const char *channel, float value)
{
    char bufTopic[20];
    char buffer[128];
    sprintf(bufTopic, "%s/%s", topic.c_str(), channel);
    sprintf(buffer, "{\"data\": %f, \"write\": true}", value);
    mqtt.publish(bufTopic, buffer, strlen(buffer));
}

