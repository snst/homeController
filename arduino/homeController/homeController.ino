// Copyright 2017 Stefan Schmidt

#include <WiFi.h>
#include <PubSubClient.h>
#include "SimpleEsp32Ble.h"
#include "HomeBle.h"
#include "HomeConfig.h"


enum eCmd {
    NONE = 0,
    PING,
    BOOST_ON,
    BOOST_OFF,
    ON,
    OFF,
    ECO,
    COMFORT,
    AUTO,
    MANUAL,
    REBOOT,
    SETTEMP,
    GETSTATUS
};

static uint8_t mqttStatus[12];
static uint8_t bleCmd[7];
static uint8_t bleCmdLen;

HomeBLE* ble;
BLEAddr bleAddrConnect; //("00:1a:22:0c:5e:15");
BLEAddr bleAddrCmd;
HomeConfig config;

WiFiClient espClient;
PubSubClient client(espClient);

void(* softReset) (void) = 0;//declare reset function at address 0

void printMem()
{
  Serial.print("mem: ");
  Serial.print(xPortGetFreeHeapSize());
  Serial.print("/");
  Serial.println(xPortGetMinimumEverFreeHeapSize());
}

void setMqttResponseStatus(uint8_t* pData, size_t length) {
    if(length==6) {
      int i = 0;
      mqttStatus[i++] = '*';
      mqttStatus[i++] = 2 + sizeof(esp_bd_addr_t) + 4;
      memcpy(&mqttStatus[i], bleAddrConnect.addr, sizeof(esp_bd_addr_t));
      i += sizeof(esp_bd_addr_t);
      memcpy(&mqttStatus[i], &pData[2], 4);
    }
}

void setMqttResponsePong() {
  mqttStatus[0] = '!';
  mqttStatus[1] = 2;  
}


void setup() {

  mqttStatus[0] = 0;
  bleCmdLen = 0;

  Serial.begin(115200);
  printMem();

  config.loadSettings();
  config.userInput(2000);

  ble = new HomeBLE();
  ble->init();

  client.setServer(config.mqtt_server.c_str(), atoi(config.mqtt_port.c_str()));
  client.setCallback(callback);

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

void connectMQTT()
{
  while (!client.connected()) {
    Serial.print("Connecting to MQTT.. ");
    if (client.connect("ESP32Client", config.mqtt_user.c_str(), config.mqtt_pw.c_str() )) {
      Serial.print("connected! Subscripe to ");
      Serial.println(config.mqtt_topic_request.c_str());  
      client.subscribe(config.mqtt_topic_request.c_str());
    } else {
      Serial.print("failed with state ");
      Serial.println(client.state());
      delay(2000);
    }
  }
}

void publishMqttStatus()
{
  if(mqttStatus[0] != 0)
  {
    Serial.print("<MQTT: ");
    Serial.print(config.mqtt_topic_status.c_str());
    if(client.publish(config.mqtt_topic_status.c_str(), mqttStatus, mqttStatus[1])) {
      Serial.println(" - OK");
    } else {
      Serial.println(" - FAILED");
    }
    mqttStatus[0] = 0; // cmd
    mqttStatus[1] = 0; // len
//    printMem();
  }
}


void callback(char* topic, byte* payload, unsigned int length) {
 
  bleCmdLen = 0;

  Serial.print("\n>MQTT: ");
  Serial.print(topic);

  Serial.print(" : ");
  /*
  for (int i = 0; i < length; i++) {
    Serial.print(payload[i], HEX);
    Serial.print(", ");
  }
  Serial.println("");
  */
  if(!strcmp(topic, config.mqtt_topic_request.c_str()) && (length >= 2) && (payload[1]==length)) {

      eCmd cmd = (eCmd) payload[0];
      payload += 2;
      length -= 2;
      bleAddrCmd.setAddr(payload);
      bleAddrCmd.print("BLE");
      payload += BTADDR_LEN;
      length -= BTADDR_LEN;
      Serial.print(" : ");
      switch(cmd) {
        case NONE: {
          Serial.println("NONE");
        } break;
        case PING: {
          setMqttResponsePong();
          Serial.println("PING");
          printMem();
        } break;
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
        } break;
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
            Serial.println("PACKET");
          }
        } break;
      }
      
    } else {
      Serial.println("corrupt packet");
    }
}


void loop() {

  connectWLAN();

  connectMQTT();
  
  client.loop();

  if(bleCmdLen > 0)
  {
    if(bleAddrConnect.isSame(bleAddrCmd)) {
//      bleAddrCmd.print("Same target addr");
    }
    else {
      bleAddrCmd.print("New target addr");
      if(!ble->isState(SimpleBLE::disconnected)) {
        bleAddrConnect.print("Disconnect target");
        ble->disconnect();
      }
      bleAddrConnect.setAddr(bleAddrCmd);
    }
    
    if(ble->isState(SimpleBLE::disconnected)) {
      bleAddrConnect.print("Connect to target");
      ble->connect(bleAddrConnect);
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
