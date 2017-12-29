// Copyright 2017 Stefan Schmidt

#ifndef __COMMON_H__
#define __COMMON_H__
#include <Arduino.h>
#include <stdarg.h>

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


#endif