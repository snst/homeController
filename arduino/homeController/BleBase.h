// Copyright 2017 Stefan Schmidt

#ifndef _BLEBASE_H__
#define _BLEBASE_H__

#include "Arduino.h"
#include <stdint.h>
#include <string.h>
#include <stdbool.h>
#include <stdio.h>
#include "nvs.h"
#include "nvs_flash.h"
#include "controller.h"
#include "bt.h"
#include "esp_gap_ble_api.h"
#include "esp_gattc_api.h"
#include "esp_gatt_defs.h"
#include "esp_bt_main.h"
#include "esp_gatt_common_api.h"
#include "BTAddr.h"

#define BTADDR_LEN (sizeof(esp_bd_addr_t))

void setMqttResponse(uint8_t* pData, size_t length);


#define APP_ID 0
#define MAX_APP 1
#define MAX_CONNECTIONS 15

class BleBase {

  public:
  enum eState { disconnected=0, connecting, disconnecting, failed, connected };
  bool isWriting;
  esp_gattc_cb_t a_gattc_cb;
  uint16_t a_gattc_if;

  typedef struct {
    BTAddr addr;
    eState state;
    uint16_t connId;
  } tConnState;

  tConnState connState[MAX_CONNECTIONS];
  SemaphoreHandle_t connStateMutex;
  
  BleBase();
  virtual ~BleBase() {}

  int getConnIndex(BTAddr &addr);
  void setConnState(BTAddr &addr, eState state, uint16_t connId);
  eState getConnState(BTAddr &addr);
  uint16_t getConnId(BTAddr &addr);
  
  bool isConnState(BTAddr &addr, eState state);

  virtual void onConnectFailed(BTAddr &addr) {
    addr.print("BleBase::onConnectFailed()", true);
  }

  virtual void onDisconnected(BTAddr &addr) {
    addr.print("BleBase::disconnected()", true);
  }

  virtual void onConnected(BTAddr &addr) {
    addr.print("BleBase::onConnected()", true);
  }

  virtual void onReceiveData(BTAddr &addr, uint8_t* pData, uint8_t len) {
    Serial.print("BleBase::onReceiveData() len=");
    Serial.println(len);
  }

  virtual void onServiceFound() {
    Serial.println("BleBase::onServiceFound()");
  }

  virtual void onWritten(bool success) {
//    Serial.print("BleBase::onWritten() ");
//    Serial.println(success ? "ok" : "failed");
  }

  bool registerNotify(BTAddr &addr, uint16_t handle);

  bool write(BTAddr &addr, uint16_t handle, uint8_t* data, uint8_t len, bool response);

  bool connect(BTAddr& addr);
  void disconnect();

  bool init();

/*
  bool isState(eState _state) {
    return state == _state;
  }

  void setState(eState _state) {
    state = _state;
  //  if(state < connected) {
      isWriting = false;
  //  }
  }
*/

  bool canWrite() {
    return true;
 //   return state==ready && !isWriting;
  }

};


#endif
