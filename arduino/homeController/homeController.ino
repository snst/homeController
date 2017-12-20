// Copyright 2017 Stefan Schmidt

#include <WiFi.h>
#include <PubSubClient.h>
#include "common.h"
#include "SimpleEsp32Ble.h"
#include "HomeBle.h"
#include "HomeConfig.h"
#include "CmdQueue.h"
#include "MqttHandler.h"

HomeBLE* ble;
HomeConfig config;
CmdQueue queue;

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

  ble = new HomeBLE();
  ble->init();

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


void nextCmd() {

  uint8_t buffer[MQTT_CMD_SIZE];
  uint8_t* payload = buffer; // CMD(1), LEN(1), BTADDR(6), PARAM(1)
  BTAddr bleAddr;
  uint8_t bleCmd[BLE_CMD_SIZE];
  uint8_t bleCmdLen = 0;

  if(!ble->isReady() || !queue.getCmd(payload)) {
    return;
  }
  
  Serial.print("RunCmd: ");
  eCmd cmd = (eCmd) payload[0];
  uint8_t length = payload[1];
  payload += 2;
  length -= 2;
  bleAddr.setAddr(payload);
  bleAddr.print("BLE", false);
  payload += BTADDR_LEN;
  length -= BTADDR_LEN;
  Serial.print(" : ");
  switch(cmd) {
    case NONE: {
      Serial.println("NONE");
    } break;
    case PING: {
      mqtt.sendResponsePong();
      Serial.println("PING");
      printMem();
    } return; // don't execute BT command
    case BOOST_ON: {
      bleCmd[0] = 0x45; bleCmd[1] = 0xff; bleCmdLen = 2;
      Serial.println("BOOST ON");
    } break;
    case BOOST_OFF: {
      bleCmd[0] = 0x45; bleCmd[1] = 0x00; bleCmdLen = 2;
      Serial.println("BOOST OFF");
    } break;
    case ON: {
      bleCmd[0] = 0x41; bleCmd[1] = 0x3c; bleCmdLen = 2;
      Serial.println("ON");
    } break;
    case OFF: {
      bleCmd[0] = 0x41; bleCmd[1] = 0x09; bleCmdLen = 2;
      Serial.println("OFF");
    } break;
    case ECO: {
      bleCmd[0] = 0x44; bleCmdLen = 1;
      Serial.println("ECO");
    } break;
    case COMFORT: {
      bleCmd[0] = 0x43; bleCmdLen = 1;
      Serial.println("COMFORT");
    } break;
    case AUTO: {
      bleCmd[0] = 0x40; bleCmd[1] = 0x00; bleCmdLen = 2;
      Serial.println("AUTO");
    } break;
    case MANUAL: {
      bleCmd[0] = 0x40; bleCmd[1] = 0x40;  bleCmdLen = 2;
      Serial.println("MANUAL");
    } break;
    case REBOOT: {
      Serial.println("REBOOT");
      softReset();
    } return;
    case SETTEMP: {
      Serial.print("SETTEMP: ");
      if(length==1) {
        bleCmd[0] = 0x41; 
        bleCmd[1] = (uint8_t) *payload;
        bleCmdLen = 2;
        Serial.println(bleCmd[1]);
      } else {
        Serial.println("PACKET");
      }
    } break;
    case GETSTATUS: {
      Serial.println("GETSTATUS");
      if(length==6) {
        bleCmd[0] = 3;
        bleCmdLen = 7;
        memcpy(&bleCmd[1], payload, 6);
      } else {
        Serial.println("ERR");
      }
    } break;
    default: {
      Serial.println("Invalid cmd");
    }
  }

  ble->writeCmd(bleAddr, bleCmd, bleCmdLen);
}

void loop() {

  connectWLAN();

  mqtt.connect();

  mqtt.execute();
  
  nextCmd();

  ble->execute();

  mqtt.publishStatus();  

  vTaskDelay(200/portTICK_PERIOD_MS);
}
