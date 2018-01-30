// Copyright 2017 Stefan Schmidt

#ifndef _COMMON_H__
#define _COMMON_H__

#include "BTAddr.h"
#include "configuration.h"

#define BLE_CMD_SIZE 7
#define MQTT_RESPONSE_SIZE 12
#define MQTT_CMD_SIZE 15

#define CONNID_INVALID 0xFFFF

#define EQ3_NOTIFICATION_HANDLE 0x421

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
    ABORT,
    CLOSE_CONNECTION,
    GETTEMP
};

enum eResponse {
  PONG = 1,
  STATE = 2,
  CONNECTION = 3,
  BME = 4
};

enum eState { 
  waiting=0, 
  disconnected, 
  connecting, 
  disconnecting, 
  failed, 
  connected 
};


typedef struct {
  uint8_t data[BLE_CMD_SIZE];
  uint8_t len;
  BTAddr addr;
} tBleCmd;

void dump(const char *str, const uint8_t *data, uint8_t len);
void p(uint8_t level, const char *fmt, ... );
void printMem();
void updateLastCmdTimestamp();
int getSleepTime();
const char *eState2Str(eState state);
void sleep(int ms);
bool isConnected();


bool runTemp();

#endif
