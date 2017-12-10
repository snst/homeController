
// Copyright 2017 Stefan Schmidt

#include <WiFi.h>
//#include "BLEDevice.h"
#include <PubSubClient.h>
#include "homeController_cfg.h"
#include "SimpleEsp32Ble.h"
#include "HomeBle.h"

static char strMqttStatus[6];
static uint8_t bleCmd[7];
static uint8_t bleCmdLen;

HomeBLE* ble;
BLEAddr bleAddr("00:1a:22:0c:5e:15");

WiFiClient espClient;
PubSubClient client(espClient);

/*
static BLEUUID serviceUUID("3e135142-654f-9090-134a-a6ff5bb77046");
static BLEUUID cmdUUID("3fa4585a-ce4a-3bad-db4b-b8df8179ea09");
static BLEUUID notificationUUID("d0e8434d-cd29-0996-af41-6c90f4e0eb2a");
static BLEAddress *pBLEServerAddress;
static BLERemoteCharacteristic* pBLERemoteCharacteristic;
static BLERemoteCharacteristic* pBLENotificationCharacteristic;
static BLEClient* pBLEClient;
static BLERemoteService* pBLERemoteService;
*/

void(* softReset) (void) = 0;//declare reset function at address 0


void printMem()
{
  Serial.print("mem: ");
  Serial.print(xPortGetFreeHeapSize());
  Serial.print("/");
  Serial.println(xPortGetMinimumEverFreeHeapSize());
}

void setMqttResponse(uint8_t* pData, size_t length) {
    if(length==6) {
      sprintf(strMqttStatus, "*%02x%02x%02x%02x", pData[2], pData[3], pData[4], pData[5]);
    }
}

void setup() {

  strMqttStatus[0] = 0;
  bleCmdLen = 0;

  Serial.begin(115200);
  printMem();

  ble = new HomeBLE();
  ble->init();

  client.setServer(MQTT_SERVER, MQTT_PORT);
  client.setCallback(callback);

  WiFi.begin(WLAN_SSID, WLAN_PASSWORD);
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

void connectMQTT()
{
  while (!client.connected()) {
    Serial.print("Connecting to MQTT.. ");
    if (client.connect("ESP32Client", MQTT_USER, MQTT_PASSWORD )) {
      Serial.println("connected!");  
      client.subscribe("eq3/mode");
    } else {
      Serial.print("failed with state ");
      Serial.println(client.state());
      delay(2000);
    }
  }
}

byte getNextVal(byte* p) {
  byte i = 0;
  i = p[0] - '0';
  i *= 10;
  i += p[i] - '0';
  return i;
}

void publishMqttStatus()
{
  if(strMqttStatus[0] != 0)
  {
    Serial.print("<MQTT: eq3/status : ");
    Serial.print(strMqttStatus);
    if(client.publish("eq3/status", strMqttStatus)) {
      Serial.println(" - OK");
    } else {
      Serial.println(" - FAILED");
    }
    strMqttStatus[0] = 0;
    printMem();
  }
}

void callback(char* topic, byte* payload, unsigned int length) {
 
  bleCmdLen = 0;

  Serial.print(">MQTT: ");
  Serial.print(topic);

  Serial.print(" : ");
  for (int i = 0; i < length; i++) {
    Serial.print(payload[i], HEX);
    Serial.print(", ");
  }
  Serial.println("");
  
  if(!strcmp(topic, "eq3/mode") && length >= 1) {
    char* c = (char*)payload;
    if(!strncmp(c, "on", length)) {
      bleCmd[0] = 0x41; bleCmd[1] = 0x3c; bleCmdLen = 2;
    }
    else if(!strncmp(c, "off", length)) {
      bleCmd[0] = 0x41; bleCmd[1] = 0x09; bleCmdLen = 2;
    }
    else if(!strncmp(c, "manual", length)) { 
      bleCmd[0] = 0x40; bleCmd[1] = 0x40;  bleCmdLen = 2;
    }
    else if(!strncmp(c, "auto", length)) {
      bleCmd[0] = 0x40; bleCmd[1] = 0x00; bleCmdLen = 2;
    }
    else if(!strncmp(c, "booston", length)) {
      bleCmd[0] = 0x45; bleCmd[1] = 0xff; bleCmdLen = 2;
    }
    else if(!strncmp(c, "boostoff", length)) {
      bleCmd[0] = 0x45; bleCmd[1] = 0x00; bleCmdLen = 2;
    }
    else if(!strncmp(c, "eco", length)) {
      bleCmd[0] = 0x44; bleCmdLen = 1;
    }
    else if(!strncmp(c, "comfort", length)) {
      bleCmd[0] = 0x43; bleCmdLen = 1;
    }
    else if(!strncmp(c, "reset", length)) { 
      softReset();
    }
    else if(!strncmp(c, "ping", length)) { 
      strcpy(strMqttStatus, "pong");
    }
    else if(!strncmp(c, "t", 1)) { 
      int t = 0;
      if(length>=2) t = payload[1] - '0';
      if(length>=3) { t *=  10; t += ( payload[2] - '0' ); }
      if(length>=4) { t *=  10; t += ( payload[3] - '0' ); }

      bleCmd[0] = 0x41; 
      bleCmd[1] = (uint8_t) ((t * 2) / 10);
      bleCmdLen = 2;
    }
    else if(!strncmp(c, "s", 1) && length==13) { 
      bleCmd[0] = 3;
      bleCmd[1] = getNextVal(payload+1); // year
      bleCmd[2] = getNextVal(payload+3); // month
      bleCmd[3] = getNextVal(payload+5); // day
      bleCmd[4] = getNextVal(payload+7); // hour
      bleCmd[5] = getNextVal(payload+9); // minute
      bleCmd[6] = getNextVal(payload+11); // second
      bleCmdLen = 7;
    }

    printMem();
  }
}


void loop() {

  connectWLAN();

  connectMQTT();
  
  client.loop();

  if(bleCmdLen > 0)
  {
    if(ble->isState(SimpleBLE::disconnected)) {
      ble->connect(bleAddr);
    }

    if(ble->canWrite()) {
      if( ble->write(0x411, bleCmd, bleCmdLen, true) ) {
        bleCmdLen = 0;
      }
    }
  }

  publishMqttStatus();  

  vTaskDelay(200/portTICK_PERIOD_MS);
}
