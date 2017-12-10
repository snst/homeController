#include "HomeBle.h"

HomeBLE::HomeBLE() {
}
    
HomeBLE::~HomeBLE() {
}

void HomeBLE::onReceiveNotify(uint8_t* pData, uint8_t len) {
    SimpleBLE::onReceiveNotify(pData, len);
    setMqttResponse(pData, len);
}


void HomeBLE::onConnected() {
  SimpleBLE::onConnected();
  registerNotify(0x421);
  setState(ready);
}
