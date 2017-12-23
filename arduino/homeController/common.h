// Copyright 2017 Stefan Schmidt

#ifndef _COMMON_H__
#define _COMMON_H__

#include "BTAddr.h"

#define VERSION_MAJOR   0
#define VERSION_MINOR   1
#define VERSION_REV     1

#define BLE_CMD_SIZE 7

#define BT_CMD_QUEUE_LEN 20
#define MQTT_CMD_SIZE 15

#define MQTT_RESPONSE_SIZE 12
#define MQTT_RESPONSE_QUEUE_LEN 20

#define CONNID_INVALID 0xFFFF

#define MUTEX_MAX_DELAY 1000

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



#endif
