// Copyright 2017 Stefan Schmidt
#ifndef __MUTEX_H__
#define __MUTEX_H__

#include "Arduino.h"

class Mutex {
public:
    Mutex();
    void lock();
    void unlock();
protected:
    SemaphoreHandle_t mutex;
};

class AutoLock {
public:
    AutoLock(Mutex &m);
    ~AutoLock();
protected:
    Mutex &mutex;
};

#endif