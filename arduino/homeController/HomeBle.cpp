// Copyright 2017 Stefan Schmidt

#include "HomeBle.h"

HomeBLE::HomeBLE()
: bleCmdLen(0) {
}
    
HomeBLE::~HomeBLE() {
}

void HomeBLE::onReceiveNotify(uint8_t* pData, uint8_t len) {
    SimpleBLE::onReceiveNotify(pData, len);
    setMqttResponseStatus(&bleAddr, pData, len);
}

void HomeBLE::onDisconnected() {
  SimpleBLE::onDisconnected();
  bleCmdLen = 0;
}


void HomeBLE::onConnected() {
  SimpleBLE::onConnected();
  registerNotify(0x421);
  setState(ready);
}

bool HomeBLE::isReady() {
  return bleCmdLen == 0;
}

void HomeBLE::writeCmd(BLEAddr& addr, uint8_t* cmd, uint8_t cmdLen) {

  if(bleAddr.isSame(addr)) {
//      bleAddrCmd.print("Same target addr");
  }
  else {
    addr.print("New target addr", true);
    if(!isState(SimpleBLE::disconnected)) {
      bleAddr.print("Disconnect target", true);
      disconnect();
    }
    bleAddr.setAddr(addr);
  }

  memcpy(bleCmd, cmd, cmdLen);
  bleCmdLen = cmdLen;
}

void HomeBLE::onWritten(bool success) {
  SimpleBLE::onWritten(success);
  bleCmdLen = 0;
}

void HomeBLE::execute() {

  if(bleCmdLen > 0)
  {
    if(isState(SimpleBLE::disconnected)) {
      bleAddr.print("Connect to target", true);
      connect(bleAddr);
    }

    if(canWrite()) {
      if( write(0x411, bleCmd, bleCmdLen, true) ) {
//        bleCmdLen = 0;
      }
    }
  }
}

