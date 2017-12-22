// Copyright 2017 Stefan Schmidt

#include <WiFi.h>
#include <PubSubClient.h>
#include "common.h"
#include "BleBase.h"
#include "BleHandler.h"
#include "HomeConfig.h"
#include "MqttHandler.h"

BleHandler ble;
HomeConfig config;
WiFiClient wifi;
PubSubClient client(wifi);
MqttHandler mqtt(client);

void(* softReset) (void) = 0;//declare reset function at address 0


void printMem()
{
  Serial.print("mem: ");
  Serial.print(xPortGetFreeHeapSize());
  Serial.print("/");
  Serial.println(xPortGetMinimumEverFreeHeapSize());
}


void setup() {
  Serial.begin(115200);
  printMem();

  config.loadSettings();
  config.userInput(2000);

  ble.init();

  mqtt.setTopicStatus(config.mqtt_topic_status.c_str());
  mqtt.setTopicRequest(config.mqtt_topic_request.c_str());
  mqtt.setServer(config.mqtt_server.c_str(), atoi(config.mqtt_port.c_str()));
  mqtt.setUser(config.mqtt_user.c_str(), config.mqtt_pw.c_str());
  
  WiFi.begin(config.wlan_ssid.c_str(), config.wlan_pw.c_str());
  printMem();
}


void connectWLAN()
{
  if(WiFi.status() != WL_CONNECTED) {
    int i=15;
    Serial.print("Connecting to WiFi..");
    while (WiFi.status() != WL_CONNECTED) {
      if(--i == 0) {
        Serial.println(" trying reboot");
        softReset();
      }
      delay(1000);
      Serial.print(".");
    }
    Serial.println(" connected!");
  }
}


void loop() {

  connectWLAN();
  taskYIELD();

  mqtt.connect();
  taskYIELD();

  mqtt.execute();
  taskYIELD();
  
  ble.execute();
  taskYIELD();

  vTaskDelay(20/portTICK_PERIOD_MS);
}
