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


struct gattc_profile_inst {
    esp_gattc_cb_t gattc_cb;
    uint16_t gattc_if;
    uint16_t app_id;
    uint16_t conn_id;
    uint16_t service_start_handle;
    uint16_t service_end_handle;
    uint16_t char_handle;
    esp_bd_addr_t remote_bda;
};


#define APP_ID 0
#define MAX_APP 1

class BleBase {

  public:
  enum eState { deinit, disconnected, connecting, disconnecting, connected, ready };
  eState state;
  bool isWriting;
  BTAddr bleAddrNew;
  BTAddr bleAddrConnected;
  struct gattc_profile_inst gattcProfile[MAX_APP]; 
  
  BleBase();
  virtual ~BleBase() {}

  virtual void onConnectFailed(BTAddr &addr) {
    addr.print("BleBase::onConnectFailed()", true);
  }

  virtual void onDisconnected(BTAddr &addr) {
    addr.print("BleBase::disconnected()", true);
  }

  virtual void onConnected(BTAddr &addr) {
    addr.print("BleBase::onConnected()", true);
  }

  virtual void onReceiveNotify(BTAddr &addr, uint8_t* pData, uint8_t len) {
    Serial.print("BleBase::onReceiveNotify() len=");
    Serial.println(len);
  }

  virtual void onReceiveIndicate(BTAddr &addr, uint8_t* pData, uint8_t len) {
    Serial.print("BleBase::onReceiveIndicate() len=");
    Serial.println(len);
  }

  virtual void onServiceFound() {
    Serial.println("BleBase::onServiceFound()");
  }

  virtual void onWritten(bool success) {
//    Serial.print("BleBase::onWritten() ");
//    Serial.println(success ? "ok" : "failed");
  }

  bool registerNotify(uint16_t handle);

  bool write(uint16_t handle, uint8_t* data, uint8_t len, bool response);

  bool connect(BTAddr& addr);
  void disconnect();

  bool init();

  bool isState(eState _state) {
    return state == _state;
  }

  void setState(eState _state) {
    state = _state;
  //  if(state < connected) {
      isWriting = false;
  //  }
  }

  bool canWrite() {
    return state==ready && !isWriting;
  }

};


#endif
