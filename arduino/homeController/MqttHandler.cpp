// Copyright 2017 Stefan Schmidt

#include "MqttHandler.h"
#include "BTAddr.h"
#include "BleHandler.h"

extern BleHandler ble;
extern MqttHandler mqtt;

void(* softReset2) (void) = 0;//declare reset function at address 0


MqttHandler::MqttHandler(PubSubClient &c)
: client(c) {
  queueMutex = xSemaphoreCreateMutex();
  queue = xQueueCreate(MQTT_RESPONSE_QUEUE_LEN, MQTT_RESPONSE_SIZE);
  client.setCallback(MqttHandler::callback);
}

void MqttHandler::addResponse(uint8_t *msg) {
  xSemaphoreTake(queueMutex, MUTEX_MAX_DELAY);
  xQueueSendToBack(queue, msg, 0);
  xSemaphoreGive(queueMutex);
/*  Serial.print("MqttHandler::addResponse(");
  Serial.print(msg[0]);
  Serial.println(")");*/
}


bool MqttHandler::get(uint8_t *msg) {
  xSemaphoreTake(queueMutex, MUTEX_MAX_DELAY);
  bool ret = (pdPASS == xQueueReceive(queue, msg, 0));
  xSemaphoreGive(queueMutex);

 /* if (ret) {
    Serial.print("MqttHandler::get(");
    Serial.print(msg[0]);
    Serial.println(")");
  }*/
  return ret;
}


void MqttHandler::sendResponseStatus(BTAddr &addr, uint8_t* pData, size_t length) {
  if (addr.isValid() && pData && length==6) {
    uint8_t msg[MQTT_RESPONSE_SIZE];
    int i = 0;
    msg[i++] = (byte)eResponse::STATE;
    msg[i++] = 2 + BT_ADDR_SIZE + 4;
    memcpy(&msg[i], addr.addr, BT_ADDR_SIZE);
    i += BT_ADDR_SIZE;
    memcpy(&msg[i], &pData[2], 4);
    addr.print("<MqttHandler::sendResponseStatus", true);
    addResponse(msg);
  }
}


void MqttHandler::sendResponsePong() {
  uint8_t msg[MQTT_RESPONSE_SIZE];
  msg[0] = (byte)eResponse::PONG;
  msg[1] = 2;  
  Serial.println("<MqttHandler::sendResponsePong()");
  addResponse(msg);
}


void MqttHandler::sendResponseConnection(BTAddr &addr, eConnectionState state) {
  if (addr.isValid()) {
    uint8_t msg[MQTT_RESPONSE_SIZE];
    msg[0] = (byte)eResponse::CONNECTION;
    msg[1] = 3 + BT_ADDR_SIZE;
    memcpy(&msg[2], addr.addr, BT_ADDR_SIZE);
    msg[8] = (byte)state;
    addr.print("<MqttHandler::sendResponseConnection()", false);
    Serial.print(" : ConnState=");
    Serial.println(msg[8]);
    addResponse(msg);
  }
}


void MqttHandler::connect()
{
  while (!client.connected()) {
    Serial.print("Connecting to MQTT.. ");
    if (client.connect("ESP32Client", user, password )) {
      Serial.print("connected!\nSubscripe to ");
      Serial.println(topicRequest);  
      client.subscribe(topicRequest);
    } else {
      Serial.print("failed with state ");
      Serial.println(client.state());
      delay(2000);
    }
  }
}


void MqttHandler::execute() {
  
  client.loop();

  uint8_t msg[MQTT_RESPONSE_SIZE];
  if (get(msg)) {
//    Serial.print("<MQTT: ");
//    Serial.print(msg[0]);
    if(client.publish(topicStatus, msg, msg[1])) {
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


void MqttHandler::callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("\n>MQTT: ");
  Serial.print(topic);
  Serial.print(", len(");
  Serial.print(length);
  Serial.println(")");
//  if(!strcmp(topic, topicRequest) && (length >= 2) && (payload[1]==length) && (length<=MQTT_CMD_SIZE)) {
  if((length >= 2) && (payload[1] == length) && (length <= MQTT_CMD_SIZE)) {
    mqtt.parseRequest(payload);
  } else {
    Serial.println("corrupt packet");
  }
}

void MqttHandler::parseRequest(uint8_t *payload) {
  // payload: CMD(1), LEN(1), BTADDR(6), PARAM(1)
  tBleCmd cmd;
  memset(&cmd, 0, sizeof(tBleCmd));
 
  eCmd inCmd = (eCmd) payload[0];
  uint8_t length = payload[1];
  payload += 2;
  length -= 2;
  Serial.print("MqttHandler::parseRequest(");
  Serial.print(inCmd);
  Serial.print(") - ");
  cmd.addr.setAddr(payload);
  cmd.addr.print("BLE", false);
  payload += BTADDR_LEN;
  length -= BTADDR_LEN;
  Serial.print(" : ");
  
  switch(inCmd) {
    
    case NONE: {
      Serial.println("NONE");
      break;
    }
    case PING: {
      Serial.println("PING");
      mqtt.sendResponsePong();
      break;
    } 
    case BOOST_ON: {
      cmd.data[0] = 0x45; 
      cmd.data[1] = 0xff; 
      cmd.len = 2;
      Serial.println("BOOST ON");
      break;
    }
    case BOOST_OFF: {
      cmd.data[0] = 0x45; 
      cmd.data[1] = 0x00; 
      cmd.len = 2;
      Serial.println("BOOST OFF");
      break;
    }
    case ON: {
      cmd.data[0] = 0x41; 
      cmd.data[1] = 0x3c; 
      cmd.len = 2;
      Serial.println("ON");
      break;
    }
    case OFF: {
      cmd.data[0] = 0x41; 
      cmd.data[1] = 0x09; 
      cmd.len = 2;
      Serial.println("OFF");
      break;
    }
    case ECO: {
      cmd.data[0] = 0x44; 
      cmd.len = 1;
      Serial.println("ECO");
      break;
    }
    case COMFORT: {
      cmd.data[0] = 0x43; 
      cmd.len = 1;
      Serial.println("COMFORT");
      break;
    }
    case AUTO: {
      cmd.data[0] = 0x40; 
      cmd.data[1] = 0x00; 
      cmd.len = 2;
      Serial.println("AUTO");
      break;
    }
    case MANUAL: {
      cmd.data[0] = 0x40; 
      cmd.data[1] = 0x40;  
      cmd.len = 2;
      Serial.println("MANUAL");
      break;
    }
    case REBOOT: {
      Serial.println("REBOOT");
      softReset2();
      break; // compiler error
    }
    case SETTEMP: {
      Serial.print("SETTEMP: ");
      if(length==1) {
        cmd.data[0] = 0x41; 
        cmd.data[1] = *payload;
        cmd.len = 2;
        Serial.println(cmd.data[1]);
      } else {
        Serial.println("PACKET");
      }
      break;
    }
    case GETSTATUS: {
      Serial.println("GETSTATUS");
      if(length==6) {
        cmd.data[0] = 3;
        cmd.len = 7;
        memcpy(&cmd.data[1], payload, 6);
      } else {
        Serial.println("ERR");
      }
      break;
    }
    case ABORT: {
      Serial.println("ABORT");
      ble.clear();
      break;
    }
    default: {
      Serial.println("Invalid cmd");
      break;
    }
  }

  if (cmd.len > 0) {
    ble.addCmd(&cmd);
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

