// Copyright 2017 Stefan Schmidt

#ifndef __MQTT_HANDLER_H__
#define __MQTT_HANDLER_H__
#include <PubSubClient.h>
#include "common.h"

class BTAddr;
class PubSubClient;

class MqttHandler {

public:
  MqttHandler(PubSubClient &c);
  void sendResponseStatus(BTAddr &addr, uint8_t* pData, size_t length);
  void sendResponseConnection(BTAddr &addr, eConnectionState state);
  void sendResponsePong();
  void connect();
  void execute();
  void setServer(const char *server, int port);
  void setUser(const char *user, const char *password);
  static void callback(char* topic, byte* payload, unsigned int length);
  void setTopicStatus(const char *topic);
  void setTopicRequest(const char *topic);
  void addResponse(uint8_t *msg);
  bool get(uint8_t *msg);
  
protected:
  xQueueHandle queue;
  PubSubClient &client;
  const char *topicStatus;
  const char *topicRequest;
  const char *user;
  const char *password;
  void parseRequest(uint8_t *payload);
};


#endif
