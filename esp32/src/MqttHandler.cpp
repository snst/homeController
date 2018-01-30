// Copyright 2017 Stefan Schmidt

#include "MqttHandler.h"
#include "BTAddr.h"
#include "BleHandler.h"
#include "MqttResponse.h"
#include "common.h"

extern BleHandler ble;
extern MqttHandler mqtt;

void(* softReset2) (void) = 0; //declare reset function at address 0


MqttHandler::MqttHandler(PubSubClient &c)
: client(c) {
  queue = xQueueCreate(MQTT_RESPONSE_QUEUE_LEN, sizeof(MqttResponse));
  client.setCallback(MqttHandler::callback);
}

void MqttHandler::addResponse(MqttResponse &msg) {
  xQueueSendToBack(queue, &msg, 0);
}


bool MqttHandler::getResponse(MqttResponse &msg) {
  bool ret = (pdPASS == xQueueReceive(queue, &msg, 0));
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


void MqttHandler::sendResponseConnection(const BTAddr &addr, eState state) {
  if (addr.isValid()) {
    MqttResponseConnState msg(addr, state);
    msg.print();
    addResponse(msg);
  }
}

void MqttHandler::sendResponseSensor(uint8_t sensorId, float temp, float humidity, float pressure)  {
  MqttResponseSensor msg(sensorId, temp, humidity, pressure);
  msg.print();
  addResponse(msg);
}

bool MqttHandler::isConnected() {
  return client.connected();
}

void MqttHandler::connect() {
  while (!client.connected()) {
   p(9, "Connecting to MQTT.. ");
    if (client.connect("ESP32Client", user, password )) {
      p(9, "connected!\nSubscripe to %s\n", topicRequest);
      client.subscribe(topicRequest);
      //sleep(500);
    } else {
      p(9, "failed with state 0x%x\n", client.state());
      sleep(1000);
    }
  }
}

void MqttHandler::publish(const char *topic, const char *payload, unsigned int length) {
  if (client.connected()) {
    client.publish(topic, payload, length);
  }
}


bool MqttHandler::execute() {
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
  return true;
}


void MqttHandler::setServer(const char *server, int port) {
    client.setServer(server, port);
}


void MqttHandler::callback(char *topic, byte *payload, unsigned int length) {
  p(9, "\n>MQTT ");
//  if(!strcmp(topic, topicRequest) && (length >= 2) && (payload[1]==length) && (length<=MQTT_CMD_SIZE)) {
  if((length >= 2) && (payload[1] == length) && (length <= MQTT_CMD_SIZE)) {
    mqtt.parseRequest(payload);
  } else {
    p(9, "corrupt packet");
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
  p(9, "->parseRequest(");
  payload += BT_ADDR_SIZE;
  length -= BT_ADDR_SIZE;
  
  switch(inCmd) {
    
    case NONE: {
      p(9, "NONE)");
      break;
    }
    case PING: {
      p(9, "PING)");
      mqtt.sendResponsePing();
      break;
    } 
    case BOOST_ON: {
      cmd.data[0] = 0x45; 
      cmd.data[1] = 0xff; 
      cmd.len = 2;
      p(9, "BOOST ON");
      break;
    }
    case BOOST_OFF: {
      cmd.data[0] = 0x45; 
      cmd.data[1] = 0x00; 
      cmd.len = 2;
      p(9, "BOOST OFF");
      break;
    }
    case ON: {
      cmd.data[0] = 0x41; 
      cmd.data[1] = 0x3c; 
      cmd.len = 2;
      p(9, "ON");
      break;
    }
    case OFF: {
      cmd.data[0] = 0x41; 
      cmd.data[1] = 0x09; 
      cmd.len = 2;
      p(9, "OFF");
      break;
    }
    case ECO: {
      cmd.data[0] = 0x44; 
      cmd.len = 1;
      p(9, "ECO");
      break;
    }
    case COMFORT: {
      cmd.data[0] = 0x43; 
      cmd.len = 1;
      p(9, "COMFORT");
      break;
    }
    case AUTO: {
      cmd.data[0] = 0x40; 
      cmd.data[1] = 0x00; 
      cmd.len = 2;
      p(9, "AUTO");
      break;
    }
    case MANUAL: {
      cmd.data[0] = 0x40; 
      cmd.data[1] = 0x40;  
      cmd.len = 2;
      p(9, "MANUAL");
      break;
    }
    case REBOOT: {
      p(9, "REBOOT)\n");
      softReset2();
      break; // compiler error
    }
    case SETTEMP: {
      p(9, "SETTEMP: ");
      if(length==1) {
        cmd.data[0] = 0x41; 
        cmd.data[1] = *payload;
        cmd.len = 2;
        p(9, "%d", cmd.data[1] * 5);
      } else {
        p(9, "invalid)\n");
      }
      break;
    }
    case GETSTATUS: {
      p(9, "GETSTATUS");
      if(length==6) {
        cmd.data[0] = 3;
        cmd.len = 7;
        memcpy(&cmd.data[1], payload, 6);
      } else {
        p(9, "invalid)\n");
      }
      break;
    }
    case ABORT: {
      p(9, "ABORT)\n");
      ble.clear();
      break;
    }
    case CLOSE_CONNECTION: {
      p(9, "CLOSE_CONNECTION)\n");
      ble.disconnect(cmd.addr);
      break;
    }
    case GETTEMP: {
      p(9, "GETTEMP)\n");
#ifdef ENABLE_TEMP_INSIDE
      runTempInside();
#endif
#ifdef ENABLE_TEMP_OUTSIDE
      runTempOutside();
#endif
      break;
    }
    default: {
      p(9, "Invalid cmd)\n");
      break;
    }
  }
  
  if (cmd.len > 0) {
    p(9, ")\n");
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
