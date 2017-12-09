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

void initBLE();
void updateBLE(uint8_t bleCmd[], uint8_t* bleCmdLen);
void setMqttResponse(uint8_t* pData, size_t length);



#endif
