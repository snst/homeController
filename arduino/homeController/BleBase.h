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

#define APP_ID 0
#define MAX_APP 1
#define MAX_CONNECTIONS 15

class BleBase {

  public:
    enum eState { disconnected=0, connecting, disconnecting, failed, connected };
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
    int getConnIndex(const BTAddr &addr);
    void setConnState(const BTAddr &addr, eState state, uint16_t connId);
    eState getConnState(const BTAddr &addr);
    uint16_t getConnId(const BTAddr &addr);
    bool isConnState(const BTAddr &addr, eState state);
    bool registerNotify(const BTAddr &addr, uint16_t handle);
    bool write(const BTAddr &addr, uint16_t handle, const uint8_t *data, uint8_t len, bool response);
    bool connect(const BTAddr &addr);
    bool init();
    const char *eState2Str(eState state) const;

    virtual void onConnectFailed(const BTAddr &addr) {
      addr.println("BleBase::onConnectFailed()");
    }

    virtual void onDisconnected(const BTAddr &addr) {
      addr.println("BleBase::disconnected()");
    }

    virtual void onConnected(const BTAddr &addr) {
      addr.println("BleBase::onConnected()");
    }

    virtual void onReceiveData(const BTAddr &addr, const uint8_t* pData, uint8_t len) {
      Serial.print("BleBase::onReceiveData(len=");
      Serial.print(len);
      Serial.println(")");
    }

    virtual void onServiceFound() {
      Serial.println("BleBase::onServiceFound()");
    }

    virtual void onWritten(bool success) {
  //    Serial.print("BleBase::onWritten() ");
  //    Serial.println(success ? "ok" : "failed");
    }
};

#endif
