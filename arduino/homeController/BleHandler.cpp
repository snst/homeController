// Copyright 2017 Stefan Schmidt

#include "BleHandler.h"
#include "common.h"
#include "MqttHandler.h"

extern MqttHandler mqtt;

BleHandler::BleHandler() {
  queue = xQueueCreate(BT_CMD_QUEUE_LEN, sizeof(tBleCmd));
}
    
BleHandler::~BleHandler() {
}

void BleHandler::onReceiveNotify(BTAddr &addr, uint8_t* pData, uint8_t len) {
    BleBase::onReceiveNotify(addr, pData, len);
    mqtt.sendResponseStatus(addr, pData, len);
}

void BleHandler::onDisconnected(BTAddr &addr) {
  BleBase::onDisconnected(addr);
  mqtt.sendResponseConnection(addr, eConnectionState::DISCONNECTED);
}

void BleHandler::onConnectFailed(BTAddr &addr) {
  BleBase::onConnectFailed(addr);
  tBleCmd cmd;
  if(getCmd(&cmd)) {
    cmd.addr.print("REMOVEIT", true);    
  }
  mqtt.sendResponseConnection(addr, eConnectionState::NORESPONSE);
}


void BleHandler::onConnected(BTAddr &addr) {
  BleBase::onConnected(addr);
  mqtt.sendResponseConnection(addr, eConnectionState::CONNECTED);
  registerNotify(0x421);
  setState(ready);
}

bool BleHandler::connect(BTAddr& addr) {
  mqtt.sendResponseConnection(addr, eConnectionState::CONNECTING);
  return BleBase::connect(addr);
}


void BleHandler::disconnect() {
  mqtt.sendResponseConnection(bleAddrConnected, eConnectionState::DISCONNECTED);
  BleBase::disconnect();
}


void BleHandler::onWritten(bool success) {
  BleBase::onWritten(success);
}


void BleHandler::execute() {

  tBleCmd cmd;
  if(peekCmd(&cmd)) {

    if(isState(BleBase::disconnected)) {
  	  connect(cmd.addr);
	    return;
	  } 

    if(isState(BleBase::ready)) {
    
      if(!bleAddrConnected.isSame(cmd.addr)) {
        Serial.println("BleHandler::execute() New BleAddr, disconnect!");
        disconnect();
        return;
      }
  
      if(canWrite()) {
      
        tBleCmd cmd;
        if(getCmd(&cmd)) {
        
          if( write(0x411, cmd.data, cmd.len, true) ) {
          }
        }
      }
    }
  }
}


bool BleHandler::getCmd(tBleCmd *cmd) {
  bool ret = (pdPASS == xQueueReceive(queue, cmd, 0));
  if (ret) {
    cmd->addr.print("BleHandler::getCmd() : ", false);
    Serial.print("  cmd[");
    Serial.print(cmd->data[0]);
    Serial.print(",");
    Serial.print(cmd->data[1]);
    Serial.println("]");
  }
  return ret;
}


bool BleHandler::peekCmd(tBleCmd *cmd) {
  bool ret = (pdPASS == xQueuePeek(queue, cmd, 0));
  if (ret) {
//    cmd->addr.print("BleHandler::peekCmd() : ", true);
  }
  return ret;
}


void BleHandler::addCmd(tBleCmd *cmd) {
  xQueueSendToBack(queue, cmd, 0);
}

bool BleHandler::hasCmd() {
	return uxQueueMessagesWaiting(queue) > 0;
}

void BleHandler::clear() {
  xQueueReset(queue);
}

