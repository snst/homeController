// Copyright 2017 Stefan Schmidt

#include "configuration.h"
#include <PubSubClient.h>
#include "BleBase.h"
#include "BleHandler.h"
#include "HomeConfig.h"
#include "MqttHandler.h"
#include "soc/rtc.h"
#include <Wire.h>
#include "common.h"
#include "SensorBme280.h"
#include "SensorHtu21d.h"

#ifdef USE_SSL
# include <WiFiClientSecure.h>
WiFiClientSecure wifiClient;
#else
# include <WiFi.h>
WiFiClient wifiClient;
#endif


BleHandler ble;
HomeConfig config;
PubSubClient mqttClient(wifiClient);
MqttHandler mqtt(mqttClient);

void(* softReset) (void) = 0; //declare reset function at address 0
typedef bool (*execute_t)();


#ifdef ENABLE_TEMP_OUTSIDE
SensorBme280 sensorTempOutside(1, BME_PIN_SDA, BME_PIN_SCL, BME_FREQUENCY, mqtt, SENSOR_ID_ENV_OUTSIDE, "envOut");
uint32_t lastUpdateTempOutside = 0;

bool runTempOutside() 
{
  p(60,"O");
  return sensorTempOutside.execute();
}
#endif

#ifdef ENABLE_TEMP_INSIDE
SensorHtu21d sensorTempInside(0, HTU_PIN_SDA, HTU_PIN_SCL, HTU_FREQUENCY, mqtt, SENSOR_ID_ENV_INSIDE, "envHome");
uint32_t lastUpdateTempInside = 0;

bool runTempInside() 
{
  p(60,"I");
  return sensorTempInside.execute();
}
#endif


bool runBLE() {
  p(60,"B");
  return isConnected() && ble.execute();
}


bool runMQTT() {
  p(60,"M");
  return isConnected() && mqtt.execute();
}


void setup() {
  Serial.begin(115200);
  p(1, "Version %d.%d.%d\n", VERSION_MAJOR, VERSION_MINOR, VERSION_REV);
  printMem();

  config.loadSettings();
  config.userInput(2000);

  ble.init();

  mqtt.setTopicStatus(config.mqtt_topic_status.c_str());
  mqtt.setTopicRequest(config.mqtt_topic_request.c_str());
  mqtt.setServer(config.mqtt_server.c_str(), atoi(config.mqtt_port.c_str()));
  mqtt.setUser(config.mqtt_user.c_str(), config.mqtt_pw.c_str());
  
  WiFi.begin(config.wlan_ssid.c_str(), config.wlan_pw.c_str());

#ifdef ENABLE_TEMP_OUTSIDE  
  sensorTempOutside.begin();
#endif

#ifdef ENABLE_TEMP_INSIDE  
  sensorTempInside.begin();
#endif

  printMem();

#ifdef USE_LOW_MHZ
  rtc_clk_cpu_freq_set(RTC_CPU_FREQ_80M);
#endif
}


bool isConnected() 
{
  if (WiFi.status() != WL_CONNECTED) {
    int i=15;
    p(1, "Connecting to WiFi..");
    while (WiFi.status() != WL_CONNECTED) {
      if(--i == 0) {
        p(1, " trying reboot\n");
        softReset();
      }
      sleep(1000);
      p(1, ".");
    }
    p(1, " connected!\n");
  }

  mqtt.connect();
  return true;
}


void runWithInterval(execute_t function, uint32_t & lastMS, uint32_t interval) 
{
  uint32_t now = millis();
  if ((now - lastMS) >= interval) {
    if (function()) {  
      lastMS = now;
    }
  }
}


void loop() 
{
  runMQTT();
  runBLE();

#ifdef ENABLE_TEMP_INSIDE
  runWithInterval(runTempInside, lastUpdateTempInside, UPDATE_INTERVAL_TEMP);
#endif

#ifdef ENABLE_TEMP_OUTSIDE
  runWithInterval(runTempOutside, lastUpdateTempOutside, UPDATE_INTERVAL_TEMP);
#endif

  sleep(getSleepTime());
}
