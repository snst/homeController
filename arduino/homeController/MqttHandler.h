// Copyright 2017 Stefan Schmidt

#ifndef __MQTT_HANDLER_H__
#define __MQTT_HANDLER_H__
#include <PubSubClient.h>
#include "common.h"

#define MQTT_RESPONSE_MAX_LEN 12

class BTAddr;
class PubSubClient;

class MqttHandler {

public:
  MqttHandler(PubSubClient &c);
  void sendResponseStatus(BTAddr* addr, uint8_t* pData, size_t length);
  void sendResponseConnection(BTAddr* addr, eConnectionState state);
  void sendResponsePong();
  void publishStatus();
  void connect();
  void execute();
  void setServer(const char *server, int port);
  void setUser(const char *user, const char *password);
  static void callback(char* topic, byte* payload, unsigned int length);
  void setTopicStatus(const char *topic);
  void setTopicRequest(const char *topic);
  
protected:
  uint8_t mqttStatus[MQTT_RESPONSE_MAX_LEN];
  PubSubClient &client;
  const char *topicStatus;
  const char *topicRequest;
  const char *user;
  const char *password;
  
};


#endif
