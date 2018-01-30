// Copyright 2017 Stefan Schmidt
#include "configuration.h"

#include "SensorEnv.h"
#include "MqttHandler.h"
#include "common.h"


SensorEnv::SensorEnv(MqttHandler & m, uint8_t theSensorId, const char *theTopic, int bus)
    : mqttHandler(m), sensorId(theSensorId), topic(theTopic), wire(bus)
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
    Serial.print("\nSensor: ");
    Serial.println(topic);
 
    Serial.print("Temperature: ");
    Serial.print(temperature);                                  // display temperature in Celsius
    Serial.println("C");

    Serial.print("Humidity:    ");
    Serial.print(humidity);                                     // display humidity in %   
    Serial.println("%");

    Serial.print("Pressure:    ");
    Serial.print(pressure);                           // display pressure in hPa
    Serial.println("hPa");

    if (temperature > 60.0f || temperature < -40.0f || humidity < 0.0f || humidity > 100.0f) {
        Serial.println("Skip invalid values.");
        return false;
    }

    mqttHandler.sendResponseSensor(sensorId, temperature, humidity, pressure);
    publishChannelData("te", temperature);
    publishChannelData("hu", humidity);
    publishChannelData("pr", pressure);

    return true;
}

void SensorEnv::init(int sda, int scl, uint32_t frequency) 
{
    wire.begin(sda, scl, frequency);
    wire.setTimeout(500);

}
