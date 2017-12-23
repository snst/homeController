// Copyright 2017 Stefan Schmidt

#ifndef __MQTT_RESPONSE_H__
#define __MQTT_RESPONSE_H__

#include "common.h"

class MqttResponse {
  public:
    uint8_t hdr[2];
    uint8_t data[MQTT_RESPONSE_SIZE-2];

    MqttResponse() {}
    
    MqttResponse(enum eResponse response, uint8_t len) {
      hdr[0] = (uint8_t) response;
      hdr[1] = 2 + len;
    }

    void setBTAddr(const BTAddr &addr) {
      memcpy(&data[0], addr.addr, BT_ADDR_SIZE);
    }
    
    virtual void print()const {
      Serial.print("<MQTT <-");
    }

    uint8_t* getBuffer() {
      return hdr;
    }

    uint8_t getSize() const {
      return hdr[1];
    }
};

class MqttResponsePing : public MqttResponse {
  public:
    MqttResponsePing() 
    : MqttResponse(eResponse::PONG, 3) {
      data[0] = VERSION_MAJOR;
      data[1] = VERSION_MINOR;
      data[2] = VERSION_REV;
    }
    void print() const {
      MqttResponse::print();
      Serial.print("Ping(v");
      Serial.print(data[0]);
      Serial.print(".");
      Serial.print(data[1]);
      Serial.print(".");
      Serial.print(data[2]);
      Serial.println(")");
    }
};



class MqttResponseConnState : public MqttResponse {
  public:
    MqttResponseConnState(const BTAddr &addr, eConnectionState state) 
    : MqttResponse(eResponse::CONNECTION, BT_ADDR_SIZE + 1) {
      setBTAddr(addr);
      data[BT_ADDR_SIZE] = (uint8_t)state;
    }

    void print() const {
      BTAddr a(data);
      char *state = "?)";
      switch ((eConnectionState)data[BT_ADDR_SIZE]) {
        case eConnectionState::CONNECTING:
          state = "CONNECTING)";
          break;
        case eConnectionState::CONNECTED:
          state = "CONNECTED)";
          break;
        case eConnectionState::DISCONNECTED:
          state = "DISCONNECTED)";
          break;
        case eConnectionState::NORESPONSE:
          state = "NORESPONSE)";
          break;
      }
      MqttResponse::print();
      Serial.print("ConnState(");
      a.println(state);
    }
};


class MqttResponseTempState : public MqttResponse {
  public:
    MqttResponseTempState(const BTAddr &addr, const uint8_t *state) 
    : MqttResponse(eResponse::STATE, BT_ADDR_SIZE + 4) {
      setBTAddr(addr);
      memcpy(&data[BT_ADDR_SIZE], state, 4);
    }

    void print() const {
      BTAddr a(data);
      MqttResponse::print();
      Serial.print("TempState(");
      Serial.print((data[BT_ADDR_SIZE+3])*5);
      a.println(")");
    }
};

#endif

