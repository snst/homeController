// Copyright 2017 Stefan Schmidt

#ifndef _COMMON_H__
#define _COMMON_H__

#include "BTAddr.h"

#define BLE_CMD_SIZE 7

#define BT_CMD_QUEUE_LEN 20
#define MQTT_CMD_SIZE 15

#define MQTT_RESPONSE_SIZE 12
#define MQTT_RESPONSE_QUEUE_LEN 20

#define CONNID_INVALID 0xFFFF


enum eCmd {
    NONE = 0,
    PING,
    BOOST_ON,
    BOOST_OFF,
    ON,
    OFF,
    ECO,
    COMFORT,
    AUTO,
    MANUAL,
    REBOOT,
    SETTEMP,
    GETSTATUS,
    ABORT
};

enum eResponse {
  PONG = 1,
  STATE = 2,
  CONNECTION = 3
};

enum eConnectionState {
  UNKOWN = 0,
  CONNECTING = 1,
  CONNECTED = 2,
  DISCONNECTED = 3,
  NORESPONSE = 4
};

typedef struct {
  uint8_t data[BLE_CMD_SIZE];
  uint8_t len;
  BTAddr addr;
} tBleCmd;


class BLEAddr;
void setMqttResponseStatus(BLEAddr* addr, uint8_t* pData, size_t length);
void setMqttResponseConnection(BLEAddr* addr, eConnectionState state);


#endif
