#include "Mutex.h"


Mutex::Mutex() {
    mutex = xSemaphoreCreateMutex();
}


void Mutex::lock() {
    xSemaphoreTake(mutex, portMAX_DELAY);
}


void Mutex::unlock() {
    xSemaphoreGive(mutex);
}

AutoLock::AutoLock(Mutex & m)
: mutex(m) {
    m.lock();
}

AutoLock::~AutoLock() {
    mutex.unlock();
}