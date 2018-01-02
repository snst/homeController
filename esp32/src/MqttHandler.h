// Copyright 2017 Stefan Schmidt

#ifndef __MQTT_HANDLER_H__
#define __MQTT_HANDLER_H__
#include <PubSubClient.h>
#include "common.h"
#include "Mutex.h"

class BTAddr;
class PubSubClient;
class MqttResponse;

class MqttHandler {

public:
  MqttHandler(PubSubClient &c);
  void sendResponseStatus(const BTAddr &addr, const uint8_t* pData, size_t length);
  void sendResponseConnection(const BTAddr &addr, eState state);
  void sendResponsePing();
  void connect();
  void execute();
  void setServer(const char *server, int port);
  void setUser(const char *user, const char *password);
  void setTopicStatus(const char *topic);
  void setTopicRequest(const char *topic);

protected:
  static void callback(char *topic, byte *payload, unsigned int length);
  void addResponse(MqttResponse &msg);
  bool getResponse(MqttResponse &msg);
  
  xQueueHandle queue;
  Mutex mutexQueue;
  PubSubClient &client;
  const char *topicStatus;
  const char *topicRequest;
  const char *user;
  const char *password;
  void parseRequest(const uint8_t *payload);
};


#endif
