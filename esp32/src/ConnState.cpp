#include "ConnState.h"


ConnState::ConnState() 
: ageCnt(0) {
    memset(&connState, 0, sizeof(connState));
}


uint8_t ConnState::count(eState state) {

    uint8_t n = 0;
    AutoLock l(mutex);
    for (int i=0; i<MAX_CONNECTIONS; i++) {
        tConnState &st = connState[i];
        if ((st.state == state) && (st.addr.isValid())) {
            n++;
        }
    }  
    return n;
}


void ConnState::reset(const BTAddr &addr) {
  AutoLock l(mutex);
  for (int i=0; i<MAX_CONNECTIONS; i++) {
    if(addr.isSame(connState[i].addr)) {
      memset(&connState[i], 0, sizeof(tConnState));
    }
  }  
}


eState ConnState::get(const BTAddr &addr) {
  AutoLock l(mutex);
  eState ret = disconnected;
  int i = getIndex(addr);
  if (i >= 0) {
    ret = connState[i].state;
  }
  return ret;
}


uint16_t ConnState::getId(const BTAddr &addr) {
  AutoLock l(mutex);
  int i = getIndex(addr);
  uint16_t connId = 0;
  if (i > 0) {
    connId = connState[i].connId;
  }
  return connId;
}


void ConnState::set(const BTAddr &addr, eState state, uint16_t connId) {
  AutoLock l(mutex);
  int i = getIndex(addr);
  if (i >= 0) {
    if (!connState[i].addr.isValid()) {
      connState[i].addr = addr;
    }
    connState[i].state = state;

    if (state == connected) {
        connState[i].age = ++ageCnt;
    }

    if (connId != CONNID_INVALID) {
      connState[i].connId = connId;
    }
  }

  p(20, "ConnState::set(%s", eState2Str(state));
  if (connId != CONNID_INVALID) {
    p(20, ", connId=0x%x", connId);
  }
  p(20, ", i=%d)", i);
  addr.println(20, "");
}


int ConnState::getIndex(const BTAddr &addr) {
  
  for (int i=0; i<MAX_CONNECTIONS; i++) {
    if (addr.isSame(connState[i].addr)) {
      return i;
    }
  }  

  for (int i=0; i<MAX_CONNECTIONS; i++) {
    if (!connState[i].addr.isValid()) {
      return i;
    }
  }  
  
  return -1;
}


bool ConnState::getOldestOpenConn(BTAddr &addr) {
    AutoLock l(mutex);
    uint16_t age = 0xFFFF;
    int selected = -1;
    for (int i=0; i<MAX_CONNECTIONS; i++) {
        if ((connState[i].age > 0) && (connState[i].age < age)) {
            age = connState[i].age;
            selected = i;
//           p("go: %d = %d\n", i, connState[i].age);
        }
    }
    if (selected >= 0) {
        addr = connState[selected].addr;
        p(20, "Oldest conn(age=%d)", connState[selected].age);
        addr.println(20, "");
        return true;
    }  
    return false;
}
