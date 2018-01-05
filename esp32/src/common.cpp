// Copyright 2017 Stefan Schmidt

#ifndef __COMMON_H__
#define __COMMON_H__
#include <Arduino.h>
#include <stdarg.h>
#include "common.h"

unsigned long lastMS = 0;

void updateLastCmdTimestamp() {
 lastMS = millis();
}

int getSleepTime() {

  unsigned long ms = millis();

  if ((ms - lastMS) > IDLE_AFTER_MS) {
    return (SLEEP_IDLE_TIME);
  } else {
    return (SLEEP_WORK_TIME);
  }
}

void p(const char *fmt, ... ) {
        char buf[128]; // resulting string limited to 128 chars
        va_list args;
        va_start (args, fmt );
        vsnprintf(buf, 128, fmt, args);
        va_end (args);
        Serial.print(buf);
}

void dump(const char *str, const uint8_t *data, uint8_t len) {
  Serial.print(str);
  Serial.print("(");
  Serial.print(len);
  Serial.print("): ");
  for(uint8_t i=0; i<len; i++) {
    Serial.print(data[i], HEX);
    Serial.print(", ");
  }
  Serial.println("");
}


void printMem()
{
  Serial.print("mem: ");
  Serial.print(xPortGetFreeHeapSize());
  Serial.print("/");
  Serial.println(xPortGetMinimumEverFreeHeapSize());
}

void sleep(int ms) {
  vTaskDelay( ms / portTICK_RATE_MS);
}



const char *eState2Str(eState state) {
  switch(state) {
    case waiting:       return "WAITING";
    case disconnected:  return "DISCONNECTED";
    case connecting:    return "CONNECTING";
    case disconnecting: return "DISCONNECTING";
    case failed:        return "FAILED";
    case connected:     return "CONNECTED";
    default:            return "??"; 
  }
}

#endif