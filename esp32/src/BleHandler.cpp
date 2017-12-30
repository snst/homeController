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
}

void BleHandler::onConnectFailed(const BTAddr &addr) {
  BleBase::onConnectFailed(addr);
  clearCmdFromQueue(addr);
}


void BleHandler::onConnected(const BTAddr &addr, uint16_t connId) {
  BleBase::onConnected(addr, connId);
  registerNotify(addr, 0x421);
  delay(50);
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
          delay(50);
        } else {
          //p("W");
        }
        addCmd(cmd);
        break;
      }
      case connecting: {
        addCmd(cmd);
        break;
      }
      case disconnecting: {
        cmd.addr.println("!!Disconnecting ??!");
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


void BleHandler::clear() {
  xSemaphoreTake(queueMutex, MUTEX_MAX_DELAY);
  xQueueReset(queue);
  xSemaphoreGive(queueMutex);
}


void BleHandler::setConnState(const BTAddr &addr, eState state, uint16_t connId) {
  BleBase::setConnState(addr, state, connId);
  mqtt.sendResponseConnection(addr, state);
}


void BleHandler::clearCmdFromQueue(const BTAddr &addr) {
  xSemaphoreTake(queueMutex, MUTEX_MAX_DELAY);
  int n = uxQueueMessagesWaiting(queue);
  for (int i=0; i<n; i++) {
    tBleCmd cmd;
    if (pdPASS == xQueueReceive(queue, &cmd, 0)) {
      if (!addr.isSame(cmd.addr)) {
        xQueueSendToBack(queue, &cmd, 0);
      } else {
        // drop cmd
      }
    }
  }
  xSemaphoreGive(queueMutex);
}
