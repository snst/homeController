// Copyright 2017 Stefan Schmidt

#include "BleHandler.h"
#include "common.h"
#include "MqttHandler.h"

extern MqttHandler mqtt;


BleHandler::BleHandler()
: msNextRound(0) {
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
  mqtt.sendResponseConnection(addr, disconnected);
}


void BleHandler::onConnecting(const BTAddr &addr) {
  BleBase::onConnecting(addr);
  mqtt.sendResponseConnection(addr, connecting);
}

void BleHandler::onConnectFailed(const BTAddr &addr) {
  BleBase::onConnectFailed(addr);
  clearCmdFromQueue(addr);
  mqtt.sendResponseConnection(addr, failed);
}


void BleHandler::onConnected(const BTAddr &addr, uint16_t connId) {
  BleBase::onConnected(addr, connId);
  registerNotify(addr, 0x421);
  mqtt.sendResponseConnection(addr, connected);
}

bool BleHandler::skipRound() {
  return millis() < msNextRound;
}

void BleHandler::setNextRound(uint16_t ms) {
  msNextRound = millis() + ms;
}


void BleHandler::execute() {

  if (!skipRound()) {

    setNextRound(100);

    tBleCmd cmd;
    AutoLock m1(mutexCmd);
    AutoLock m2(mutexBT);
    if(getCmd(cmd)) {
      eState state = connState.get(cmd.addr);
      switch (state) {
        case waiting: {
          if (!isConnecting()) {
            if (hasFreeConnections()) {
              connect(cmd.addr);
              setNextRound(500);
            } else {
              closeOldestConnection();
              setNextRound(500);
            }
          } else {
            //p("W");
          }
          addCmdIntern(cmd);
          break;
        }
        case connecting: {
          addCmdIntern(cmd);
          break;
        }
        case disconnecting: {
          cmd.addr.println("!!Disconnecting??");
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
            addCmdIntern(cmd); 
          }
          break;
        }
      }
    }
  }
}


bool BleHandler::getCmd(tBleCmd &cmd) {
  bool ret = (pdPASS == xQueueReceive(queue, &cmd, 0));
  return ret;
}


void BleHandler::addCmd(const tBleCmd &cmd) {
  AutoLock l(mutexCmd);
  addCmdIntern(cmd);
}


void BleHandler::addCmdIntern(const tBleCmd &cmd) {
  xQueueSendToBack(queue, &cmd, 0);
}


void BleHandler::clear() {
  AutoLock l(mutexCmd);
  xQueueReset(queue);
}


void BleHandler::clearCmdFromQueue(const BTAddr &addr) {
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
}

