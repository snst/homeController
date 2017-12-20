// Copyright 2017 Stefan Schmidt

#include "CmdQueue.h"
#include "HomeBLE.h"
#include "MqttHandler.h"

extern HomeBLE ble;
extern MqttHandler mqtt;

CmdQueue::CmdQueue() { 
  data = (uint8_t *)malloc(QUEUE_LEN*MQTT_CMD_SIZE);
  clear();
}
    
CmdQueue::~CmdQueue() { 
}

void CmdQueue::clear() {
  count = 0;
  memset(data, 0, MQTT_CMD_SIZE*QUEUE_LEN);
  ri = 0;
}

bool CmdQueue::addCmd(uint8_t *cmd) {
  if(count>=QUEUE_LEN) {
    Serial.println("Queue::addCmd() FULL");
    return false;
  }
  for(uint8_t j=0; j<QUEUE_LEN; j++) {
    uint8_t i = (j+ri) % QUEUE_LEN;
    if(data[i*MQTT_CMD_SIZE+1] == 0) {
      memcpy(&data[i*MQTT_CMD_SIZE], cmd, cmd[1]);
      count++;
      Serial.print("Queue::addCmd(");
      Serial.print(data[i*MQTT_CMD_SIZE]);
      Serial.print("/");
      Serial.print(data[i*MQTT_CMD_SIZE+1]);
      Serial.print("), i=");
      Serial.print(i);
      Serial.print(", cnt=");
      Serial.print(count);
      return true;
    }
  }
  return false;
}


bool CmdQueue::getCmd(uint8_t *cmd) {
  if(count>0) {
    for(uint8_t j=0; j<QUEUE_LEN; j++) {
      uint8_t i = (j+ri) % QUEUE_LEN;
      if(data[i*MQTT_CMD_SIZE+1] != 0) {
        ri = i;
        memcpy(cmd, &data[i*MQTT_CMD_SIZE], data[i*MQTT_CMD_SIZE+1]);
        data[i*MQTT_CMD_SIZE+1] = 0;
        count--;
        Serial.print("Queue::getCmd(), i=");
        Serial.print(i);
        Serial.print(", cnt=");
        Serial.println(count);
        return true;
      }
    }
  }
  return false;
}

void CmdQueue::execute() {
  
  uint8_t buffer[MQTT_CMD_SIZE];
  uint8_t* payload = buffer; // CMD(1), LEN(1), BTADDR(6), PARAM(1)
  BTAddr bleAddr;
  uint8_t bleCmd[BLE_CMD_SIZE];
  uint8_t bleCmdLen = 0;

  if(!ble.isReady() || !getCmd(payload)) {
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
//      printMem();
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
        Serial.println("ERR");
      }
    } break;
    case ABORT: {
      Serial.println("ABORT");
      clear();
    } break;
    default: {
      Serial.println("Invalid cmd");
    }
  }

  ble.writeCmd(bleAddr, bleCmd, bleCmdLen);
}

