// Copyright 2017 Stefan Schmidt

#ifndef _CMDQUEUE_H__
#define _CMDQUEUE_H__

#include "common.h"

#define QUEUE_LEN 5

class CmdQueue {
  protected:
  
    uint8_t count;
    uint8_t data[QUEUE_LEN][MQTT_CMD_SIZE];
    uint8_t ri;
  
  public:
    CmdQueue()
    : count(0), ri(0) { 
    }
    ~CmdQueue() { }

    bool addCmd(uint8_t *cmd) {
      Serial.println("cmd:add");
      if(count>=QUEUE_LEN) {
        Serial.println("Queue::addCmd() FULL");
        return false;
      }
      for(uint8_t j=0; j<QUEUE_LEN; j++) {
        uint8_t i = (j+ri) % QUEUE_LEN;
        if(data[i][1] == 0) {
          memcpy(data[i], cmd, cmd[1]);
          count++;
          Serial.print("Queue::addCmd(), i=");
          Serial.print(i);
          Serial.print(", cnt=");
          Serial.println(count);
          return true;
        }
      }
      return false;
    }

    bool getCmd(uint8_t *cmd) {
      if(count>0) {
        for(uint8_t j=0; j<QUEUE_LEN; j++) {
          uint8_t i = (j+ri) % QUEUE_LEN;
          if(data[i][1] != 0) {
            ri = i;
            memcpy(cmd, data[i], data[i][1]);
            data[i][1] = 0;
            count--;
            Serial.print("Queue::getCmd(), i=");
            Serial.print(i);
            Serial.print(", cnt=");
            Serial.println(count);
            return true;
          }
        }
      }
      return false;
    }
};

#endif
