// Copyright 2017 Stefan Schmidt

#include "MqttHandler.h"
#include "BTAddr.h"
#include "BleHandler.h"
#include "MqttResponse.h"

extern BleHandler ble;
extern MqttHandler mqtt;

void(* softReset2) (void) = 0; //declare reset function at address 0


MqttHandler::MqttHandler(PubSubClient &c)
: client(c) {
  queueMutex = xSemaphoreCreateMutex();
  queue = xQueueCreate(MQTT_RESPONSE_QUEUE_LEN, sizeof(MqttResponse));
  client.setCallback(MqttHandler::callback);
}

void MqttHandler::addResponse(MqttResponse &msg) {
  xSemaphoreTake(queueMutex, MUTEX_MAX_DELAY);
  xQueueSendToBack(queue, &msg, 0);
  xSemaphoreGive(queueMutex);
}


bool MqttHandler::getResponse(MqttResponse &msg) {
  xSemaphoreTake(queueMutex, MUTEX_MAX_DELAY);
  bool ret = (pdPASS == xQueueReceive(queue, &msg, 0));
  xSemaphoreGive(queueMutex);
  return ret;
}


void MqttHandler::sendResponseStatus(const BTAddr &addr, const uint8_t *pData, size_t length) {
  if (addr.isValid() && pData && length==6) {
    MqttResponseTempState msg(addr, &pData[2]);
    msg.print();
    addResponse(msg);
  }
}


void MqttHandler::sendResponsePing() {
  MqttResponsePing msg;
  msg.print();
  addResponse(msg);
}


void MqttHandler::sendResponseConnection(const BTAddr &addr, eConnectionState state) {
  if (addr.isValid()) {
    MqttResponseConnState msg(addr, state);
    msg.print();
    addResponse(msg);
  }
}


void MqttHandler::connect() {
  while (!client.connected()) {
    Serial.print("Connecting to MQTT.. ");
    if (client.connect("ESP32Client", user, password )) {
      Serial.print("connected!\nSubscripe to ");
      Serial.println(topicRequest);  
      client.subscribe(topicRequest);
      delay(500);
    } else {
      Serial.print("failed with state ");
      Serial.println(client.state());
      delay(1000);
    }
  }
}


void MqttHandler::execute() {
  client.loop();
  MqttResponse msg;
  if (getResponse(msg)) {
//    dump("Publish", msg.getBuffer(), msg.getSize());
    if(client.publish(topicStatus, msg.getBuffer(), msg.getSize())) {
//      Serial.println(" - OK");
    } else {
//      Serial.println(" - FAILED");
    }
  }
  client.loop();
}


void MqttHandler::setServer(const char *server, int port) {
    client.setServer(server, port);
}


void MqttHandler::callback(char *topic, byte *payload, unsigned int length) {
  Serial.print("\n>MQTT ");
//  if(!strcmp(topic, topicRequest) && (length >= 2) && (payload[1]==length) && (length<=MQTT_CMD_SIZE)) {
  if((length >= 2) && (payload[1] == length) && (length <= MQTT_CMD_SIZE)) {
    mqtt.parseRequest(payload);
  } else {
    Serial.println("corrupt packet");
  }
}

void MqttHandler::parseRequest(const uint8_t *payload) {
  // payload: CMD(1), LEN(1), BTADDR(6), PARAM(1)
  updateLastCmdTimestamp();
  tBleCmd cmd;
  memset(&cmd, 0, sizeof(tBleCmd));
 
  eCmd inCmd = (eCmd) payload[0];
  uint8_t length = payload[1];
  payload += 2;
  length -= 2;
  cmd.addr.setAddr(payload);
  Serial.print("->parseRequest(");
  payload += BT_ADDR_SIZE;
  length -= BT_ADDR_SIZE;
  
  switch(inCmd) {
    
    case NONE: {
      Serial.println("NONE)");
      break;
    }
    case PING: {
      Serial.println("PING)");
      mqtt.sendResponsePing();
      break;
    } 
    case BOOST_ON: {
      cmd.data[0] = 0x45; 
      cmd.data[1] = 0xff; 
      cmd.len = 2;
      Serial.print("BOOST ON");
      break;
    }
    case BOOST_OFF: {
      cmd.data[0] = 0x45; 
      cmd.data[1] = 0x00; 
      cmd.len = 2;
      Serial.print("BOOST OFF");
      break;
    }
    case ON: {
      cmd.data[0] = 0x41; 
      cmd.data[1] = 0x3c; 
      cmd.len = 2;
      Serial.print("ON");
      break;
    }
    case OFF: {
      cmd.data[0] = 0x41; 
      cmd.data[1] = 0x09; 
      cmd.len = 2;
      Serial.print("OFF");
      break;
    }
    case ECO: {
      cmd.data[0] = 0x44; 
      cmd.len = 1;
      Serial.print("ECO");
      break;
    }
    case COMFORT: {
      cmd.data[0] = 0x43; 
      cmd.len = 1;
      Serial.print("COMFORT");
      break;
    }
    case AUTO: {
      cmd.data[0] = 0x40; 
      cmd.data[1] = 0x00; 
      cmd.len = 2;
      Serial.print("AUTO");
      break;
    }
    case MANUAL: {
      cmd.data[0] = 0x40; 
      cmd.data[1] = 0x40;  
      cmd.len = 2;
      Serial.print("MANUAL");
      break;
    }
    case REBOOT: {
      Serial.println("REBOOT)");
      softReset2();
      break; // compiler error
    }
    case SETTEMP: {
      Serial.print("SETTEMP: ");
      if(length==1) {
        cmd.data[0] = 0x41; 
        cmd.data[1] = *payload;
        cmd.len = 2;
        Serial.print(cmd.data[1] * 5);
      } else {
        Serial.println("invalid)");
      }
      break;
    }
    case GETSTATUS: {
      Serial.print("GETSTATUS");
      if(length==6) {
        cmd.data[0] = 3;
        cmd.len = 7;
        memcpy(&cmd.data[1], payload, 6);
      } else {
        Serial.println("invalid)");
      }
      break;
    }
    case ABORT: {
      Serial.println("ABORT)");
      ble.clear();
      break;
    }
    default: {
      Serial.println("Invalid cmd)");
      break;
    }
  }
  
  if (cmd.len > 0) {
    cmd.addr.println(")");
    ble.addCmd(cmd);
  }
}


void MqttHandler::setTopicStatus(const char *topic) {
  topicStatus = topic;
}


void MqttHandler::setTopicRequest(const char *topic) {
  topicRequest = topic;
}


void MqttHandler::setUser(const char *user, const char *password) {
  this->user = user;
  this->password = password;
}

