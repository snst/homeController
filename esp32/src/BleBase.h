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
#include "common.h"
#include "ConnState.h"
#include "Mutex.h"

#define APP_ID 0
#define MAX_APP 1

class BleBase {

  public:

    BleBase();
    virtual ~BleBase() {}
    bool init();
    void disconnect(const BTAddr &addr);
    bool connect(const BTAddr &addr);
    bool isConnecting();
    bool hasFreeConnections();
    void closeOldestConnection();
    bool write(const BTAddr &addr, uint16_t handle, const uint8_t *data, uint8_t len, bool response);
    virtual void onConnectFailed(const BTAddr &addr);
    virtual void onDisconnected(const BTAddr &addr);
    virtual void onConnected(const BTAddr &addr, uint16_t connId);
    virtual void onConnecting(const BTAddr &addr);
    virtual void onReceiveData(const BTAddr &addr, const uint8_t* pData, uint8_t len);
    virtual void onServiceFound();
    virtual void onWritten(bool success);
    bool registerNotify(const BTAddr &addr, uint16_t handle);

  protected:
    esp_gattc_cb_t a_gattc_cb;
    uint16_t a_gattc_if;
    ConnState connState;
    Mutex mutexBT;

    static void esp_gap_cb(esp_gap_ble_cb_event_t event, esp_ble_gap_cb_param_t *param);
    static void gattc_profile_event_handler(esp_gattc_cb_event_t event, esp_gatt_if_t gattc_if, esp_ble_gattc_cb_param_t *param);
};

#endif
