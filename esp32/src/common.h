// Copyright 2017 Stefan Schmidt

#ifndef _COMMON_H__
#define _COMMON_H__

#include "BTAddr.h"

#define VERSION_MAJOR   0
#define VERSION_MINOR   2
#define VERSION_REV     6

#define MAX_OPEN_CONNECTIONS 4

#define MAX_CONNECTIONS 15


#define BLE_CMD_SIZE 7

#define BT_CMD_QUEUE_LEN 20
#define MQTT_CMD_SIZE 15

#define MQTT_RESPONSE_SIZE 12
#define MQTT_RESPONSE_QUEUE_LEN 20

#define CONNID_INVALID 0xFFFF

#define IDLE_AFTER_MS (30*1000)
#define SLEEP_IDLE_TIME 250
#define SLEEP_WORK_TIME 10

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
    CLOSE_CONNECTION
};

enum eResponse {
  PONG = 1,
  STATE = 2,
  CONNECTION = 3
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
void p(const char *fmt, ... );
void printMem();
void updateLastCmdTimestamp();
int getSleepTime();
const char *eState2Str(eState state);
void sleep(int ms);


#endif
