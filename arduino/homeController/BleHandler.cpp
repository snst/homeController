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

void BleHandler::onReceiveData(BTAddr &addr, uint8_t* pData, uint8_t len) {
    BleBase::onReceiveData(addr, pData, len);
    mqtt.sendResponseStatus(addr, pData, len);
}

void BleHandler::onDisconnected(BTAddr &addr) {
  BleBase::onDisconnected(addr);
  mqtt.sendResponseConnection(addr, eConnectionState::DISCONNECTED);
}

void BleHandler::onConnectFailed(BTAddr &addr) {
  Serial.println("BleHandler::onConnectFailed");
  BleBase::onConnectFailed(addr);
 /* tBleCmd cmd;
  if(getCmd(&cmd)) {
    cmd.addr.print("REMOVEIT", true);    
  }*/
  mqtt.sendResponseConnection(addr, eConnectionState::NORESPONSE);
}


void BleHandler::onConnected(BTAddr &addr) {
  registerNotify(addr, 0x421);
  taskYIELD();
  BleBase::onConnected(addr);
  mqtt.sendResponseConnection(addr, eConnectionState::CONNECTED);
//  setConnState(addr, ready, CONNID_INVALID);
}

bool BleHandler::connect(BTAddr& addr) {
  mqtt.sendResponseConnection(addr, eConnectionState::CONNECTING);
  return BleBase::connect(addr);
}


void BleHandler::disconnect() {
//  mqtt.sendResponseConnection(bleAddrConnected, eConnectionState::DISCONNECTED);
  BleBase::disconnect();
}


void BleHandler::onWritten(bool success) {
  BleBase::onWritten(success);
}


void BleHandler::execute() {

  tBleCmd cmd;
  if(getCmd(&cmd)) {

    eState state = getConnState(cmd.addr);
    switch (state) {
      case disconnected: {
        connect(cmd.addr);
        cmd.addr.print("disconnected: Re-add cmd", true);
        addCmd(&cmd);
        break;
      }
      case connecting: {
//        cmd.addr.print("connecting: Re-add cmd", true);
        addCmd(&cmd);
        break;
      }
      case disconnecting: {
        break;
      }
      case connected: {
      //if(canWrite()) 
        {
          if (!write(cmd.addr, 0x411, cmd.data, cmd.len, true) ) {
            cmd.addr.print("write failed: Re-add cmd", true);
            addCmd(&cmd);
          }
        }
        break;
      }
    }
  }
}


bool BleHandler::getCmd(tBleCmd *cmd) {
  xSemaphoreTake(queueMutex, MUTEX_MAX_DELAY);
  bool ret = (pdPASS == xQueueReceive(queue, cmd, 0));
  xSemaphoreGive(queueMutex);
  /*if (ret) {
    cmd->addr.print("BleHandler::getCmd() : ", false);
    Serial.print("  cmd[");
    Serial.print(cmd->data[0]);
    Serial.print(",");
    Serial.print(cmd->data[1]);
    Serial.println("]");
  }*/
  return ret;
}


bool BleHandler::peekCmd(tBleCmd *cmd) {
  xSemaphoreTake(queueMutex, MUTEX_MAX_DELAY);
  bool ret = (pdPASS == xQueuePeek(queue, cmd, 0));
  xSemaphoreGive(queueMutex);
  if (ret) {
//    cmd->addr.print("BleHandler::peekCmd() : ", true);
  }
  return ret;
}


void BleHandler::addCmd(tBleCmd *cmd) {
  xSemaphoreTake(queueMutex, MUTEX_MAX_DELAY);
  xQueueSendToBack(queue, cmd, 0);
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

