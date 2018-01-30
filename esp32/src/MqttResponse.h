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
      p(19, "<MQTT <-");
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
      p(19, "Ping(v%d.%d.%d)\n",data[0], data[1], data[2]);
    }
};

class MqttResponseSensor : public MqttResponse {
  public:
    MqttResponseSensor(uint8_t sensorId, float temp, float humidity, float pressure) 
    : MqttResponse(eResponse::BME, 4) {
      int8_t t1 = (int8_t) temp;
      int8_t t2 = (int8_t)((temp-t1) * 100.0f);
      data[0] = sensorId;
      data[1] = (uint8_t) t1;
      data[2] = (uint8_t) t2;
      data[3] = (uint8_t) humidity;
    }
    void print() const {
      MqttResponse::print();
      p(19, "Sensor\n");
    }
};


class MqttResponseConnState : public MqttResponse {
  public:
    MqttResponseConnState(const BTAddr &addr, eState state) 
    : MqttResponse(eResponse::CONNECTION, BT_ADDR_SIZE + 1) {
      setBTAddr(addr);
      data[BT_ADDR_SIZE] = (uint8_t)state;
    }

    void print() const {
      BTAddr a(data);
      eState state = (eState)data[BT_ADDR_SIZE];
      MqttResponse::print();
      p(19, "ConnState(%s)", eState2Str(state));
      a.println(19, "");
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
      p(19, "TempState(%d", (data[BT_ADDR_SIZE+3])*5);
      a.println(19, ")");
    }
};

#endif

