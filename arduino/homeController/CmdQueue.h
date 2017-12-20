// Copyright 2017 Stefan Schmidt

#ifndef __CMDQUEUE_H__
#define __CMDQUEUE_H__

#include "Arduino.h"
#include <stdint.h>
#include "common.h"


#define QUEUE_LEN 20

class CmdQueue {
  protected:
    uint8_t count;
    uint8_t *data;//[QUEUE_LEN][MQTT_CMD_SIZE];
    uint8_t ri;
    void clear();
  
  public:
    CmdQueue();
    ~CmdQueue();
    bool addCmd(uint8_t *cmd);
    bool getCmd(uint8_t *cmd);
    void execute();
};

#endif
