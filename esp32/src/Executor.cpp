// Copyright 2017 Stefan Schmidt
#include "Executor.h"
#include "common.h"

Executor::Executor(uint32_t _interval)
: interval(_interval), nextRun(0), isInit(false)
{
}

void Executor::loop()
{
    if (!isInit) {
        init();
        isInit = true;
    }

    uint32_t now = millis();
//    p(1, "loop now=%u, next=%u\n", now, nextRun);
    if (now > nextRun) {
//        p(1, "RUN!\n");
        uint32_t delay = execute();
        updateNextRun(delay);
    }
}

void Executor::updateNextRun(uint32_t delay)
{
    uint32_t now = millis();
    nextRun = (delay == 0) ? (now + interval) : (now + delay);
//    p(1, "updateNextRun: now=%u, delay=%u, next=%u\n", now, delay, nextRun);
}

void Executor::forceExecute()
{
    uint32_t delay = execute();
    updateNextRun(delay);
}
