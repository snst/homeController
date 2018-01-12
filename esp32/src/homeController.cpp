// Copyright 2017 Stefan Schmidt

#include <WiFi.h>
#include <PubSubClient.h>
#include "common.h"
#include "BleBase.h"
#include "BleHandler.h"
#include "HomeConfig.h"
#include "MqttHandler.h"
#include "soc/rtc.h"


BleHandler ble;
HomeConfig config;
WiFiClient wifi;
PubSubClient client(wifi);
MqttHandler mqtt(client);

void(* softReset) (void) = 0;//declare reset function at address 0


void taskBT( void * pvParameters ){
 
  sleep(1000);
  while(true) {
    sleep(getSleepTime());
    ble.execute();
//    Serial.print(".");
  }
  vTaskDelete(NULL);  
}

void taskMQTT( void * pvParameters ){
 
  sleep(1000);
  while(true) {
    sleep(getSleepTime());
    mqtt.connect();
    mqtt.execute();
    //Serial.print(",");
  }
  vTaskDelete(NULL);  
}


void setup() {
  Serial.begin(115200);
  p("Version %d.%d.%d\n", VERSION_MAJOR, VERSION_MINOR, VERSION_REV);
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

  rtc_clk_cpu_freq_set(RTC_CPU_FREQ_80M);

  xTaskCreate(&taskBT, "taskBT", 5120, NULL, 1, NULL);
  xTaskCreate(&taskMQTT, "taskMQTT", 5120, NULL, 1, NULL);
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
      sleep(1000);
      Serial.print(".");
    }
    Serial.println(" connected!");
  }
}


void loop() {

  connectWLAN();

  sleep(1000);
}
