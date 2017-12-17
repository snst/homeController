// Copyright 2017 Stefan Schmidt

#include "HomeBle.h"
#include "common.h"

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
  setMqttResponseConnection(&bleAddr, eConnectionState::DISCONNECTED);
}

void HomeBLE::onConnectFailed() {
  SimpleBLE::onConnectFailed();
  setMqttResponseConnection(&bleAddr, eConnectionState::NORESPONSE);
  bleCmdLen = 0; // clear current command
}


void HomeBLE::onConnected() {
  SimpleBLE::onConnected();
  setMqttResponseConnection(&bleAddr, eConnectionState::CONNECTED);
  registerNotify(0x421);
  setState(ready);
}

bool HomeBLE::connect(BLEAddr& addr) {
  setMqttResponseConnection(&bleAddr, eConnectionState::CONNECTING);
  return SimpleBLE::connect(addr);
}

void HomeBLE::disconnect() {
  setMqttResponseConnection(&bleAddr, eConnectionState::DISCONNECTED);
  SimpleBLE::disconnect();
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
    //bleAddr.setAddr(addr);
    // connect(bleAddr);
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
//      bleAddr.print("Connect to target", true);
      connect(bleAddr);
    }

    if(canWrite()) {
      if( write(0x411, bleCmd, bleCmdLen, true) ) {
//        bleCmdLen = 0;
      }
    }
  }
}

