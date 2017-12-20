// Copyright 2017 Stefan Schmidt

#ifndef _COMMON_H__
#define _COMMON_H__

#define MQTT_CMD_SIZE 15
#define BLE_CMD_SIZE 7

#define BT_ADDR_SIZE 6

static void(* softReset) (void) = 0;//declare reset function at address 0


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

class BLEAddr;
void setMqttResponseStatus(BLEAddr* addr, uint8_t* pData, size_t length);
void setMqttResponseConnection(BLEAddr* addr, eConnectionState state);


#endif
