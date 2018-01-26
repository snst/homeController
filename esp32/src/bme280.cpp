#include "configuration.h"

#ifdef USE_BME280
#include "bme280.h"
#include "MqttHandler.h"
#include "common.h"
#include <Wire.h>
#include <BME280_t.h>

BME280<> BMESensor; 


Bme280::Bme280(MqttHandler & m)
    : mqttHandler(m) {
}

void Bme280::init() {
  Wire.begin(21,22); // sda, scl
  BMESensor.begin();
}

bool Bme280::execute() {

    Serial.println("+bme");
    BMESensor.refresh();                                                  // read current sensor data
    Serial.println("-bme");

    Serial.print("\nTemperature: ");
    Serial.print(BMESensor.temperature);                                  // display temperature in Celsius
    Serial.println("C");

    Serial.print("Humidity:    ");
    Serial.print(BMESensor.humidity);                                     // display humidity in %   
    Serial.println("%");

    Serial.print("Pressure:    ");
    Serial.print(BMESensor.pressure  / 100.0F);                           // display pressure in hPa
    Serial.println("hPa");

    mqttHandler.sendResponseBme(BMESensor.temperature, BMESensor.humidity, BMESensor.pressure  / 100.0F);

    char buffer[128];
    sprintf(buffer, "{\"data\": %f, \"write\": true}", BMESensor.temperature);
    mqttHandler.publish("bme/te", buffer, strlen(buffer));
    Serial.println(buffer);
    sprintf(buffer, "{\"data\": %f, \"write\": true}", BMESensor.humidity);
    mqttHandler.publish("bme/hu", buffer, strlen(buffer));
    Serial.println(buffer);
    sprintf(buffer, "{\"data\": %f, \"write\": true}", BMESensor.pressure / 100.0f);
    mqttHandler.publish("bme/pre", buffer, strlen(buffer));
    Serial.println(buffer);

    return true;
}
#endif