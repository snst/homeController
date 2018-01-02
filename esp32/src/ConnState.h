#ifndef __CONNSTATE_H__
#define __CONNSTATE_H__

#include "Arduino.h"
#include "common.h"
#include "BTAddr.h"
#include "Mutex.h"

class ConnState {
    public:
        ConnState();
        virtual void set(const BTAddr &addr, eState state, uint16_t connId);
        void reset(const BTAddr &addr);
        eState get(const BTAddr &addr);
        uint16_t getId(const BTAddr &addr);
        uint8_t count(eState state);
        bool getOldestOpenConn(BTAddr &addr);

    protected:
        int getIndex(const BTAddr &addr);
        typedef struct {
            BTAddr addr;
            eState state;
            uint16_t connId;
            uint16_t age;
        } tConnState;

        tConnState connState[MAX_CONNECTIONS];
        Mutex mutex;
        uint16_t ageCnt;
};

#endif