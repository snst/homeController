// Copyright 2017 Stefan Schmidt

#include "BleHandler.h"
#include "common.h"
#include "MqttHandler.h"

extern MqttHandler mqtt;


BleHandler::BleHandler() {
  queueMutex = xSemaphoreCreateMutex();
  queue = xQueueCreate(BT_CMD_QUEUE_LEN, sizeof(tBleCmd));
}
    
BleHandler::~BleHandler() {
}

void BleHandler::onReceiveData(const BTAddr &addr, const uint8_t *pData, uint8_t len) {
    BleBase::onReceiveData(addr, pData, len);
    mqtt.sendResponseStatus(addr, pData, len);
}

void BleHandler::onDisconnected(const BTAddr &addr) {
  BleBase::onDisconnected(addr);
  mqtt.sendResponseConnection(addr, eConnectionState::DISCONNECTED);
}

void BleHandler::onConnectFailed(const BTAddr &addr) {
  BleBase::onConnectFailed(addr);
  mqtt.sendResponseConnection(addr, eConnectionState::NORESPONSE);
}


void BleHandler::onConnected(const BTAddr &addr) {
  BleBase::onConnected(addr);
  registerNotify(addr, 0x421);
  taskYIELD();
  mqtt.sendResponseConnection(addr, eConnectionState::CONNECTED);
}


void BleHandler::execute() {
  tBleCmd cmd;
  if(getCmd(cmd)) {
    eState state = getConnState(cmd.addr);
    switch (state) {
      case queued: {
        if(canConnect()) {
          connect(cmd.addr);
//          cmd.addr.println("New state connecting: Reinsert cmd");
          mqtt.sendResponseConnection(cmd.addr, eConnectionState::CONNECTING);
          delay(100);
        } else {
          p("W");
        }
        addCmd(cmd);
        break;
      }
      case connecting: {
        addCmd(cmd);
        break;
      }
      case disconnecting: {
        cmd.addr.println("!!Disconnecting");
        setConnState(cmd.addr, BleBase::disconnected, CONNID_INVALID);
        // todo cmd
        // drop command
        break;
      }
      case disconnected: {
        cmd.addr.println("!!Disconnected");
        break;
      }
      case failed: {
        cmd.addr.println("!!Connect failed");
        setConnState(cmd.addr, BleBase::disconnected, CONNID_INVALID);
        // drop command
        break;
      }
      case connected: {
        if (!write(cmd.addr, 0x411, cmd.data, cmd.len, true) ) {
          cmd.addr.println("write failed: Reinsert cmd");
          addCmd(cmd); 
        }
        break;
      }
    }
  }
}


bool BleHandler::getCmd(tBleCmd &cmd) {
  xSemaphoreTake(queueMutex, MUTEX_MAX_DELAY);
  bool ret = (pdPASS == xQueueReceive(queue, &cmd, 0));
  xSemaphoreGive(queueMutex);
  return ret;
}


void BleHandler::addCmd(const tBleCmd &cmd) {
  xSemaphoreTake(queueMutex, MUTEX_MAX_DELAY);
  xQueueSendToBack(queue, &cmd, 0);
  xSemaphoreGive(queueMutex);
}


bool BleHandler::hasCmd() {
  xSemaphoreTake(queueMutex, MUTEX_MAX_DELAY);
  bool ret = uxQueueMessagesWaiting(queue) > 0;
  xSemaphoreGive(queueMutex);
	return ret;
}


void BleHandler::clear() {
  xSemaphoreTake(queueMutex, MUTEX_MAX_DELAY);
  xQueueReset(queue);
  xSemaphoreGive(queueMutex);
}



void BleHandler::sendAllConnStates() {

  for (int i=0; i<MAX_CONNECTIONS; i++) {
    if (connState[i].addr.isValid()) {
 //     mqtt.sendResponseConnection(connState[i].addr, connState[i].state);
    }
  }
}