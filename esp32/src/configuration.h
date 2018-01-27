// Copyright 2017 Stefan Schmidt
#ifndef _CONFIGURATION_H_
#define _CONFIGURATION_H_


#define VERSION_MAJOR   0
#define VERSION_MINOR   3
#define VERSION_REV     0

//#define USE_SSL
//#define USE_TASKS
#define USE_BME280
#define USE_HTU21D
//#define USE_LOW_MHZ

#define SENSOR_ID_ENV_INSIDE 1
#define SENSOR_ID_ENV_OUTSIDE 2

#define PIN_SCL 22 //orange
#define PIN_SDA 21 //yellow


#define MAX_OPEN_CONNECTIONS 4
#define MAX_CONNECTIONS 15

#define BT_CMD_QUEUE_LEN 20
#define MQTT_RESPONSE_QUEUE_LEN 20

#define IDLE_AFTER_MS (30*1000)
#define SLEEP_IDLE_TIME 100
#define SLEEP_WORK_TIME 10

#define UPDATE_INTERVAL_TEMP (5*60*1000)



#endif