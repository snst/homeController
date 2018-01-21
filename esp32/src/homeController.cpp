// Copyright 2017 Stefan Schmidt

#include "configuration.h"
#include <PubSubClient.h>
#include "BleBase.h"
#include "BleHandler.h"
#include "HomeConfig.h"
#include "MqttHandler.h"
#include "soc/rtc.h"
#include "common.h"

#ifdef USE_SSL
# include <WiFiClientSecure.h>
WiFiClientSecure wifiClient;
#else
# include <WiFi.h>
WiFiClient wifiClient;
#endif

#ifdef USE_BME280
# include "Bme280.h"
#endif

BleHandler ble;
HomeConfig config;
PubSubClient mqttClient(wifiClient);
MqttHandler mqtt(mqttClient);

void(* softReset) (void) = 0; //declare reset function at address 0
typedef bool (*execute_t)();

uint32_t lastUpdateBLE = 0;
uint32_t lastUpdateMQTT = 0;


#ifdef USE_BME280
uint32_t lastUpdateTemp = 0;
Bme280 bme(mqtt);
#endif


bool runTemp() {
#ifdef USE_BME280
  Serial.print("T");
  return isConnected() && bme.execute();
#else 
  return true;
#endif
}


bool runBLE() {
  Serial.print("T");
  return isConnected() && ble.execute();
}


bool runMQTT() {
  Serial.print("M");
  return isConnected() && mqtt.execute();
}


#ifdef USE_TASKS
# ifdef USE_BME280
void taskTemp(void *pvParameters) {
  while(true) {
    sleep(UPDATE_INTERVAL_TEMP);
    runTemp();
  }
  vTaskDelete(NULL);  
}
# endif


void taskBT( void * pvParameters ) {
  sleep(1000);
  while(true) {
    sleep(getSleepTime());
    runBLE();
  }
  vTaskDelete(NULL);  
}


void taskMQTT( void * pvParameters ) {
  sleep(1000);
  while(true) {
   sleep(getSleepTime());
   runMQTT();
  }
  vTaskDelete(NULL);  
}
#endif


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

#ifdef USE_BME280
  bme.init();
#endif

  printMem();

#ifdef USE_LOW_MHZ
  rtc_clk_cpu_freq_set(RTC_CPU_FREQ_80M);
#endif

#ifdef USE_TASKS
  xTaskCreate(&taskBT, "taskBT", 10*1024, NULL, 1, NULL);
  xTaskCreate(&taskMQTT, "taskMQTT", 10*1024, NULL, 1, NULL);
# ifdef USE_BME280
  xTaskCreate(&taskTemp, "taskTemp", 10*1024, NULL, 1, NULL);
# endif
#endif
}


bool isConnected() {

  if (WiFi.status() != WL_CONNECTED) {
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

  mqtt.connect();
  return true;
}


void runWithInterval(execute_t exe, uint32_t & lastMS, uint32_t interval) {

  uint32_t now = millis();
  if ((now - lastMS) >= interval) {
    if (exe()) {  
      lastMS = now;
    }
  }
}


void loop() {

#ifndef USE_TASK_MQTT
  //runWithInterval(runMQTT, lastUpdateMQTT, getSleepTime());
  runMQTT();
#endif

#ifndef USE_TASK_BLE
  //runWithInterval(runBLE, lastUpdateBLE, getSleepTime());
  runBLE();
#endif

#if defined(USE_BME280) && !defined(USE_TASK_TEMP)
  runWithInterval(runTemp, lastUpdateTemp, UPDATE_INTERVAL_TEMP);
#endif
sleep(getSleepTime());
}
