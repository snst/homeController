#ifndef _hc_bt_h__
#define _hc_bt_h__

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

class BLEAddr
{
  public:
    esp_bd_addr_t addr;
    
    BLEAddr() {}

    BLEAddr(char* strAddr) {
      uint8_t* a = (uint8_t*) addr;
      sscanf(strAddr, "%x:%x:%x:%x:%x:%x", &a[0], &a[1], &a[2], &a[3], &a[4], &a[5]);
    }
};

#define APP_ID 0
#define MAX_APP 1

class SimpleBLE {

  public:
  enum eState { deinit, disconnected, connecting, connected, ready };
  eState state;

  struct gattc_profile_inst gattcProfile[MAX_APP]; 

  
  SimpleBLE();
  virtual ~SimpleBLE() {}

  virtual void onDisconnected() {
    Serial.println("SimpleBLE::disconnected()");
  }

  virtual void onConnected() {
    Serial.println("SimpleBLE::onConnected()");
  }

  virtual void onReceiveNotify(uint8_t* pData, uint8_t len) {
    Serial.print("SimpleBLE::onReceiveNotify() len=");
    Serial.println(len);
  }

  virtual void onReceiveIndicate(uint8_t* pData, uint8_t len) {
    Serial.print("SimpleBLE::onReceiveIndicate() len=");
    Serial.println(len);
  }

  virtual void onServiceFound() {
    Serial.println("SimpleBLE::onServiceFound()");
  }

  virtual bool registerNotify(uint16_t handle);

  virtual bool write(uint16_t handle, uint8_t* data, uint8_t len, bool response);

  virtual bool connect(BLEAddr& addr);

  virtual bool init();

  bool isState(eState _state) {
    return state == _state;
  }

  void setState(eState _state) {
    state = _state;
  }

};

void setBLE(SimpleBLE* pSimpleBLE);


#endif
