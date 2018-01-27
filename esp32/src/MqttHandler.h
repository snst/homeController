// Copyright 2017 Stefan Schmidt

#ifndef __MQTT_HANDLER_H__
#define __MQTT_HANDLER_H__
#include <PubSubClient.h>
#include "common.h"

class BTAddr;
class PubSubClient;
class MqttResponse;

class MqttHandler {

public:
  MqttHandler(PubSubClient &c);
  void sendResponseStatus(const BTAddr &addr, const uint8_t* pData, size_t length);
  void sendResponseConnection(const BTAddr &addr, eState state);
  void sendResponsePing();
  void sendResponseSensor(uint8_t sensorId, float temp, float humidity, float pressure);
  void connect();
  bool execute();
  void setServer(const char *server, int port);
  void setUser(const char *user, const char *password);
  void setTopicStatus(const char *topic);
  void setTopicRequest(const char *topic);
  bool isConnected();
  void publish(const char *topic, const char *payload, unsigned int length);

protected:
  static void callback(char *topic, byte *payload, unsigned int length);
  void addResponse(MqttResponse &msg);
  bool getResponse(MqttResponse &msg);
  
  xQueueHandle queue;
  PubSubClient &client;
  const char *topicStatus;
  const char *topicRequest;
  const char *user;
  const char *password;
  void parseRequest(const uint8_t *payload);
};


#endif
