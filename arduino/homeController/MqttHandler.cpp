// Copyright 2017 Stefan Schmidt

#include "MqttHandler.h"
#include "BTAddr.h"
#include "CmdQueue.h"

extern CmdQueue queue;

MqttHandler::MqttHandler(PubSubClient &c)
: client(c) {
  mqttStatus[0] = 0;
  client.setCallback(MqttHandler::callback);
}


void MqttHandler::sendResponseStatus(BTAddr* addr, uint8_t* pData, size_t length) {
  if(length==6) {
    int i = 0;
    mqttStatus[i++] = (byte)eResponse::STATE;
    mqttStatus[i++] = 2 + BT_ADDR_SIZE + 4;
    memcpy(&mqttStatus[i], addr->addr, BT_ADDR_SIZE);
    i += BT_ADDR_SIZE;
    memcpy(&mqttStatus[i], &pData[2], 4);
    addr->print("Mqtt status from", true);
  }
}


void MqttHandler::sendResponsePong() {
  mqttStatus[0] = (byte)eResponse::PONG;
  mqttStatus[1] = 2;  
}


void MqttHandler::sendResponseConnection(BTAddr* addr, eConnectionState state) {
  mqttStatus[0] = (byte)eResponse::CONNECTION;
  mqttStatus[1] = 3 + BT_ADDR_SIZE;
  memcpy(&mqttStatus[2], addr->addr, BT_ADDR_SIZE);
  mqttStatus[8] = (byte)state;
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


void MqttHandler::publishStatus()
{
  if(mqttStatus[0] != 0)
  {
    Serial.print("<MQTT: ");
    Serial.print(topicStatus);
    if(client.publish(topicStatus, mqttStatus, mqttStatus[1])) {
      Serial.println(" - OK");
    } else {
      Serial.println(" - FAILED");
    }
    mqttStatus[0] = 0; // cmd
    mqttStatus[1] = 0; // len
  }
}


void MqttHandler::execute() {
    client.loop();
    publishStatus();
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
    queue.addCmd(payload);
  } else {
    Serial.println("corrupt packet");
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

