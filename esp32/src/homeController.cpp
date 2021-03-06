// Copyright 2017 Stefan Schmidt

#include "configuration.h"
#include <PubSubClient.h>
#include "BleBase.h"
#include "BleHandler.h"
#include "HomeConfig.h"
#include "MqttHandler.h"
#include "soc/rtc.h"
#include "common.h"
#include "SensorBme280.h"
#include "SensorHtu21d.h"
#include "DeviceI2CHard.h"
#include "DeviceI2CSoft.h"
#include "DataSink.h"


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
DeviceI2CSoft i2cBME(BME_PIN_SCL, BME_PIN_SDA);
DataSink sinkOutside(mqtt, SENSOR_ID_ENV_OUTSIDE, "envOut");
SensorBme280 sensorTempOutside(&i2cBME, &sinkOutside, UPDATE_INTERVAL_TEMP);
#endif

#ifdef ENABLE_TEMP_INSIDE
DeviceI2CHard i2cHTU(HTU_PIN_SCL, HTU_PIN_SDA);
DataSink sinkInside(mqtt, SENSOR_ID_ENV_INSIDE, "envHome");
SensorHtu21d sensorTempInside(&i2cHTU, &sinkInside, UPDATE_INTERVAL_TEMP);
#endif

void measureTemperature()
{
#ifdef ENABLE_TEMP_INSIDE
      sensorTempInside.forceExecute();
#endif
#ifdef ENABLE_TEMP_OUTSIDE
      sensorTempOutside.forceExecute();
#endif
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

  printMem();

#ifdef USE_LOW_MHZ
  rtc_clk_cpu_freq_set(RTC_CPU_FREQ_80M);
#endif
}


void connect() 
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
}

void loop() 
{
  connect();
  if (mqtt.connect()) {
    mqtt.execute();
    ble.execute();

#ifdef ENABLE_TEMP_INSIDE
    sensorTempInside.loop();
#endif

#ifdef ENABLE_TEMP_OUTSIDE
    sensorTempOutside.loop();
#endif
  }
  sleep(getSleepTime());
}
