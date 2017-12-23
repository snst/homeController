// Copyright 2017 Stefan Schmidt

#include "BleBase.h"
#include "common.h"


#define GATTC_TAG "GATTC_DEMO"

#define PROFILE_NUM      1
#define INVALID_HANDLE   0

static void esp_gap_cb(esp_gap_ble_cb_event_t event, esp_ble_gap_cb_param_t *param);
static void esp_gattc_cb(esp_gattc_cb_event_t event, esp_gatt_if_t gattc_if, esp_ble_gattc_cb_param_t *param);
static void gattc_profile_event_handler(esp_gattc_cb_event_t event, esp_gatt_if_t gattc_if, esp_ble_gattc_cb_param_t *param);

static BleBase* pBLE = nullptr;

static void gattc_profile_event_handler(esp_gattc_cb_event_t event, esp_gatt_if_t gattc_if, esp_ble_gattc_cb_param_t *param)
{
    esp_ble_gattc_cb_param_t *p_data = (esp_ble_gattc_cb_param_t *)param;

    switch (event) {
    case ESP_GATTC_REG_EVT: {
        Serial.println("#ESP_GATTC_REG_EVT, profile");
    } break;

    case ESP_GATTC_CONNECT_EVT: {
        esp_err_t mtu_ret = esp_ble_gattc_send_mtu_req (gattc_if, p_data->connect.conn_id);
        BTAddr a(p_data->connect.remote_bda);
        a.print("#ESP_GATTC_CONNECT_EVT ", true);
        pBLE->setConnState(a, BleBase::connected, p_data->connect.conn_id);
        pBLE->onConnected(a);
    } break;

    case ESP_GATTC_OPEN_EVT: {
        BTAddr a(p_data->open.remote_bda);
        a.print("#ESP_GATTC_OPEN_EVT ", false);
        Serial.println( (param->open.status == ESP_GATT_OK) ? " OK" : " FAILED");

        if (param->open.status != ESP_GATT_OK){
            pBLE->setConnState(a, BleBase::failed, CONNID_INVALID);
            pBLE->onConnectFailed(a);
        } else {
//            pBLE->setState(BleBase::connected);
//            pBLE->onConnected();
        }
    } break;

    case ESP_GATTC_CFG_MTU_EVT: {
        Serial.println("#ESP_GATTC_CFG_MTU_EVT ");
        
  /*      if (param->cfg_mtu.status != ESP_GATT_OK){
            Serial.println("failed");
        } else {
//          Serial.println("ok");
          pBLE->setState(BleBase::connected);
          pBLE->bleAddrConnected = pBLE->bleAddrNew;
          pBLE->onConnected(pBLE->bleAddrConnected);
        }        */
    } break;

    case ESP_GATTC_SEARCH_RES_EVT: {
        Serial.print("#ESP_GATTC_SEARCH_RES_EVT) srvc_id=0x");
    } break;

    case ESP_GATTC_SEARCH_CMPL_EVT: {
        Serial.println("#ESP_GATTC_SEARCH_CMPL_EVT ");
    /*    if (p_data->search_cmpl.status != ESP_GATT_OK) {
            Serial.println("failed");
        } else {
            Serial.println("ok");
            pBLE->setState(BleBase::ready);
            pBLE->onServiceFound();
        }*/
    } break;

    case ESP_GATTC_REG_FOR_NOTIFY_EVT: {
          Serial.println("#ESP_GATTC_REG_FOR_NOTIFY_EVT ??");
    } break;

    case ESP_GATTC_NOTIFY_EVT: {
      BTAddr a(p_data->notify.remote_bda);
      a.print("#ESP_GATTC_NOTIFY_EVT ", true);
      if (p_data->notify.is_notify) {
        pBLE->onReceiveData(a, p_data->notify.value, p_data->notify.value_len);
      }
    } break;

    case ESP_GATTC_WRITE_DESCR_EVT: {
          Serial.println("#ESP_GATTC_WRITE_DESCR_EVT ??");
    } break;

    case ESP_GATTC_SRVC_CHG_EVT: {
        Serial.println("#ESP_GATTC_SRVC_CHG_EVT ??");
    } break;
    case ESP_GATTC_WRITE_CHAR_EVT: {
      //Serial.print("ESP_GATTC_WRITE_CHAR_EVT) ");
      pBLE->isWriting = false;
      pBLE->onWritten(p_data->write.status == ESP_GATT_OK);
    } break;

    case ESP_GATTC_DISCONNECT_EVT: {
      BTAddr a(p_data->disconnect.remote_bda);
      a.print("#ESP_GATTC_DISCONNECT_EVT ", true);

      BleBase::eState state = pBLE->getConnState(a);
      if (state == BleBase::connected) { // only send disconnect, if connected before
        pBLE->setConnState(a, BleBase::disconnected, CONNID_INVALID);
        pBLE->onDisconnected(a);
      }
  } break;

    default:
      Serial.println("#gattc_profile_event_handler:default)");
        break;
    }
}

static void esp_gap_cb(esp_gap_ble_cb_event_t event, esp_ble_gap_cb_param_t *param)
{
    uint8_t *adv_name = NULL;
    uint8_t adv_name_len = 0;
//    Serial.print("SBLE::esp_gap_cp(");

    switch (event) {
    case ESP_GAP_BLE_SCAN_PARAM_SET_COMPLETE_EVT: {
        Serial.println("#ESP_GAP_BLE_SCAN_PARAM_SET_COMPLETE_EVT");
        //the unit of the duration is second
        uint32_t duration = 30;
        esp_ble_gap_start_scanning(duration);
    } break;

    case ESP_GAP_BLE_SCAN_START_COMPLETE_EVT: {
        Serial.print("#ESP_GAP_BLE_SCAN_START_COMPLETE_EVT ");
        //scan start complete event to indicate scan start successfully or failed
        if (param->scan_start_cmpl.status != ESP_BT_STATUS_SUCCESS) {
            Serial.print("failed err=0x");
            Serial.println(param->scan_start_cmpl.status, HEX);
        } else {
            Serial.println("ok");
        }
    } break;

    case ESP_GAP_BLE_SCAN_RESULT_EVT: {
        Serial.println("#ESP_GAP_BLE_SCAN_RESULT_EVT");
        esp_ble_gap_cb_param_t *scan_result = (esp_ble_gap_cb_param_t *)param;
        switch (scan_result->scan_rst.search_evt) {
            case ESP_GAP_SEARCH_INQ_RES_EVT: {
                Serial.println("##ESP_GAP_SEARCH_INQ_RES_EVT)");
    //            esp_log_buffer_hex(GATTC_TAG, scan_result->scan_rst.bda, 6);
            //    ESP_LOGI(GATTC_TAG, "searched Adv Data Len %d, Scan Response Len %d", scan_result->scan_rst.adv_data_len, scan_result->scan_rst.scan_rsp_len);
                adv_name = esp_ble_resolve_adv_data(scan_result->scan_rst.ble_adv,
                                                    ESP_BLE_AD_TYPE_NAME_CMPL, &adv_name_len);
            //  ESP_LOGI(GATTC_TAG, "searched Device Name Len %d", adv_name_len);
                esp_log_buffer_char(GATTC_TAG, adv_name, adv_name_len);
            // ESP_LOGI(GATTC_TAG, "\n");
                if (adv_name != NULL) {
                /*  if (strlen(remote_device_name) == adv_name_len && strncmp((char *)adv_name, remote_device_name, adv_name_len) == 0) {
                //        ESP_LOGI(GATTC_TAG, "searched device %s\n", remote_device_name);
                        if (connect == false) {
                            connect = true;
                //           ESP_LOGI(GATTC_TAG, "connect to the remote device.");
                            esp_ble_gap_stop_scanning();
                            esp_ble_gattc_open(pBLE->gattcProfile[APP_ID].gattc_if, scan_result->scan_rst.bda, true);
                        }
                    }*/
                }
            } break;
            case ESP_GAP_SEARCH_INQ_CMPL_EVT: {
                Serial.println("##ESP_GAP_SEARCH_INQ_CMPL_EVT)");
            } break;
            
            default: {
                Serial.println("##default)");
            } break;
        }
    } break;

    case ESP_GAP_BLE_SCAN_STOP_COMPLETE_EVT: {
          Serial.println("#ESP_GAP_BLE_SCAN_STOP_COMPLETE_EVT");
        if (param->scan_stop_cmpl.status != ESP_BT_STATUS_SUCCESS){
  //          ESP_LOGE(GATTC_TAG, "scan stop failed, error status = %x", param->scan_stop_cmpl.status);
            break;
        }
//        ESP_LOGI(GATTC_TAG, "stop scan successfully");
    } break;

    case ESP_GAP_BLE_ADV_STOP_COMPLETE_EVT: {
          Serial.println("#ESP_GAP_BLE_ADV_STOP_COMPLETE_EVT");
        if (param->adv_stop_cmpl.status != ESP_BT_STATUS_SUCCESS){
         //   ESP_LOGE(GATTC_TAG, "adv stop failed, error status = %x", param->adv_stop_cmpl.status);
            break;
        }
//        ESP_LOGI(GATTC_TAG, "stop adv successfully");
    } break;

    case ESP_GAP_BLE_UPDATE_CONN_PARAMS_EVT: {
          Serial.println("#ESP_GAP_BLE_UPDATE_CONN_PARAMS_EVT");
       /* ESP_LOGI(GATTC_TAG, "update connetion params status = %d, min_int = %d, max_int = %d,conn_int = %d,latency = %d, timeout = %d",
                  param->update_conn_params.status,
                  param->update_conn_params.min_int,
                  param->update_conn_params.max_int,
                  param->update_conn_params.conn_int,
                  param->update_conn_params.latency,
                  param->update_conn_params.timeout);*/
    } break;
    default:
        Serial.println("default)");
        break;
    }
}

static void esp_gattc_cb(esp_gattc_cb_event_t event, esp_gatt_if_t gattc_if, esp_ble_gattc_cb_param_t *param)
{
    if (event == ESP_GATTC_REG_EVT) {
        Serial.print("#ESP_GATTC_REG_EVT app_id=0x");
        Serial.print(param->reg.app_id, HEX);

        if (param->reg.status == ESP_GATT_OK) {
            Serial.println(" ok");
            if(param->reg.app_id<MAX_APP) {
                pBLE->a_gattc_if = gattc_if;
            }
        } else {
            Serial.println(" failed");
//            ESP_LOGI(GATTC_TAG, "reg app failed, app_id %04x, status %d", param->reg.app_id, param->reg.status);
            return;
        }
    }

    /* If the gattc_if equal to profile A, call profile A cb handler,
     * so here call each profile's callback */
    do {
        int idx;
        for (idx = 0; idx < MAX_APP; idx++) {
            if (gattc_if == ESP_GATT_IF_NONE || // ESP_GATT_IF_NONE, not specify a certain gatt_if, need to call every profile cb function 
                    gattc_if == pBLE->a_gattc_if) {
                if (pBLE->a_gattc_cb) {
                    pBLE->a_gattc_cb(event, gattc_if, param);
                }
            }
        }
    } while (0);
}

bool BleBase::init()
{
  //  Serial.println("+BleBase::init");
    pBLE = this;

    a_gattc_cb = gattc_profile_event_handler;
    a_gattc_if = ESP_GATT_IF_NONE;
    
    // Initialize NVS.
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES) {
    //    ESP_ERROR_CHECK(nvs_flash_erase());
      //  ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK( ret );

    esp_bt_controller_config_t bt_cfg = BT_CONTROLLER_INIT_CONFIG_DEFAULT();
    ret = esp_bt_controller_init(&bt_cfg);
    if (ret) {
      Serial.println("-initBLE1");
      return false;
    }

    ret = esp_bt_controller_enable(ESP_BT_MODE_BLE);
    if (ret) {
      Serial.println("-initBLE2");
      return false;
    }

    ret = esp_bluedroid_init();
    if (ret) {
      Serial.println("-initBLE3");
      return false;
    }

    ret = esp_bluedroid_enable();
    if (ret) {
       Serial.println("-initBLE4");
       return false;
    }

    //register the  callback function to the gap module
    ret = esp_ble_gap_register_callback(esp_gap_cb);
    if (ret){
      Serial.println("-initBLE5");
      return false;
    }

    //register the callback function to the gattc module
    ret = esp_ble_gattc_register_callback(esp_gattc_cb);
    if(ret){
      Serial.println("-initBLE6");
      return false;
    }

    ret = esp_ble_gattc_app_register(APP_ID);
    if (ret){
      Serial.println("-initBLE7");
    }
  
    esp_err_t local_mtu_ret = esp_ble_gatt_set_local_mtu(500);
    if (local_mtu_ret){
      Serial.println("-initBLE8");
    }

    Serial.println("BleBase::init: ok");
//    setState(disconnected);

    vTaskDelay(200/portTICK_PERIOD_MS);
    return true;
}

bool BleBase::write(BTAddr &addr, uint16_t handle, uint8_t* data, uint8_t len, bool response) {
    Serial.print("BleBase::write(handle=0x");
    Serial.print(handle, HEX);
    Serial.print(", len=");
    Serial.print(len);

    uint16_t connId = getConnId(addr);

    esp_err_t errRc = ::esp_ble_gattc_write_char(
        pBLE->a_gattc_if,
        connId,
        handle,
        len,
        data,
        response ? ESP_GATT_WRITE_TYPE_RSP : ESP_GATT_WRITE_TYPE_NO_RSP,
        ESP_GATT_AUTH_REQ_NONE
    );

    if (errRc != ESP_OK) {
        Serial.println(") failed");
        return false;
    } else {
        Serial.println(") ok");
        return true;
    }
}

bool BleBase::connect(BTAddr& addr) {
    addr.print("BleBase::connect()",true);
    setConnState(addr, connecting, CONNID_INVALID);
    esp_err_t errRc = esp_ble_gattc_open(a_gattc_if, addr.addr, true);
//    Serial.println(errRc == ESP_OK ? ") ok" : ") failed");

    if(errRc == ESP_OK) {
//      setConnState(addr, connecting, CONNID_INVALID);
      return true;
    } else {
      setConnState(addr, disconnected, CONNID_INVALID);
      return false;
    }
}

void BleBase::disconnect() {
  Serial.println("BleBase::disconnect() TODO");
 // setConnState(disconnecting);  
 // esp_ble_gattc_close(gattcProfile[APP_ID].gattc_if, gattcProfile[APP_ID].conn_id);
}


bool BleBase::registerNotify(BTAddr &addr, uint16_t handle) {
    Serial.print("BleBase::registerNotify() handle=0x");
    Serial.println(handle, HEX);
    esp_ble_gattc_register_for_notify (pBLE->a_gattc_if, addr.addr, handle);
    return true;
}


BleBase::BleBase() 
: isWriting(false) {
  connStateMutex = xSemaphoreCreateMutex();
  memset(&connState, 0, sizeof(connState));
 //   Serial.println("BleBase()");
}

int BleBase::getConnIndex(BTAddr &addr) {
  
  for (int i=0; i<MAX_CONNECTIONS; i++) {
    if(addr.isSame(connState[i].addr)) {
      return i;
    }
  }  

  for (int i=0; i<MAX_CONNECTIONS; i++) {
    if(!connState[i].addr.isValid()) {
      return i;
    }
  }  
  
  return -1;
}

void BleBase::setConnState(BTAddr &addr, eState state, uint16_t connId) {

  xSemaphoreTake(connStateMutex, MUTEX_MAX_DELAY);
  int i = getConnIndex(addr);
  if (i >= 0) {
    if (!connState[i].addr.isValid()) {
      connState[i].addr = addr;
    }
    connState[i].state = state;

    if (connId != CONNID_INVALID) {
      connState[i].connId = connId;
    }
  }
  xSemaphoreGive(connStateMutex);

  addr.print("setConnState: ", false);
  Serial.print(", state=");
  Serial.print(state);
  if (connId != CONNID_INVALID) {
    Serial.print(", connId=0x");
    Serial.print(connId, HEX);
  }
  Serial.print(", i=");
  Serial.println(i);
}

BleBase::eState BleBase::getConnState(BTAddr &addr) {

  xSemaphoreTake(connStateMutex, MUTEX_MAX_DELAY);
  eState ret = disconnected;
  int i = getConnIndex(addr);

  if (i >= 0) {
    ret = connState[i].state;
  }
  xSemaphoreGive(connStateMutex);
/*
  addr.print("getConnState: ", false);
  Serial.print(", state=");
  Serial.print(ret);
  Serial.print(", i=");
  Serial.println(i);
*/
  return ret;
}

uint16_t BleBase::getConnId(BTAddr &addr) {
  
  xSemaphoreTake(connStateMutex, MUTEX_MAX_DELAY);
  int i = getConnIndex(addr);
  uint16_t connId = 0;

  if (i > 0) {
    connId = connState[i].connId;
  }

  xSemaphoreGive(connStateMutex);
  
  addr.print("getConnId: ", false);
  Serial.print(", connId=0x");
  Serial.print(connId, HEX);
  Serial.print(", i=");
  Serial.println(i);
  return connId;
}

bool BleBase::isConnState(BTAddr &addr, eState state) {
  return getConnState(addr) == state;
}

