// Copyright 2017 Stefan Schmidt
#ifndef _EXECUTOR_H_
#define _EXECUTOR_H_

#include <Arduino.h>

class Executor
{
  public:
    Executor(uint32_t _interval);
    virtual ~Executor() {}
    void loop();
    void updateNextRun(uint32_t delay);
    virtual void init() = 0;
    virtual uint32_t execute() = 0;
    void forceExecute();

    uint32_t interval;
    uint32_t nextRun;
    bool isInit;
};

#endif
