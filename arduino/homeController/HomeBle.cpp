#include "HomeBle.h"

HomeBLE::HomeBLE() {
}
    
HomeBLE::~HomeBLE() {
}

void HomeBLE::onReceiveNotify(uint8_t* pData, uint8_t len) {
    SimpleBLE::onReceiveNotify(pData, len);
    setMqttResponse(pData, len);
}

void HomeBLE::onServiceFound() {
    Serial.println("HomeBLE::onServiceFound");
//    registerNotify(0x421);
}

void HomeBLE::onConnected() {
  Serial.println("HomeBLE::onConnected()");
  registerNotify(0x421);
  setState(ready);
}
