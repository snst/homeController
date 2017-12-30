// Copyright 2017 Stefan Schmidt

#include "BleBase.h"
#include "common.h"


#define GATTC_TAG "GATTC_ESP"
#define PROFILE_NUM      1
#define INVALID_HANDLE   0

extern "C" uint8_t gatt_find_i_tcb_free(void);
static BleBase* pBLE = nullptr;

void BleBase::gattc_profile_event_handler(esp_gattc_cb_event_t event, esp_gatt_if_t gattc_if, esp_ble_gattc_cb_param_t *param)
{
    esp_ble_gattc_cb_param_t *p_data = (esp_ble_gattc_cb_param_t *)param;

    switch (event) {
    case ESP_GATTC_REG_EVT: {
        p("#ESP_GATTC_REG_EVT app_id=0x%x\n", param->reg.app_id);

        if (param->reg.status == ESP_GATT_OK) {
            pBLE->a_gattc_if = gattc_if;
        } else {
            p("#ESP_GATTC_REG_EVT FAILED\n");
        }
    } break;

    case ESP_GATTC_CONNECT_EVT: { // physical connection set up
        esp_err_t mtu_ret = esp_ble_gattc_send_mtu_req (gattc_if, p_data->connect.conn_id);
        BTAddr a(p_data->connect.remote_bda);
//        a.println("#ESP_GATTC_CONNECT_EVT ");
        pBLE->onConnected(a, p_data->connect.conn_id);
    } break;

    case ESP_GATTC_OPEN_EVT: {
        BTAddr a(p_data->open.remote_bda);
//        a.print("#ESP_GATTC_OPEN_EVT ");
//        Serial.println( (param->open.status == ESP_GATT_OK) ? " OK" : " FAILED");

        if (param->open.status != ESP_GATT_OK){
            pBLE->onConnectFailed(a);
        } else {
//            pBLE->setState(BleBase::connected);
//            pBLE->onConnected();
        }
    } break;

    case ESP_GATTC_QUEUE_FULL_EVT: {
        Serial.println("#ESP_GATTC_QUEUE_FULL_EVT");
    } break;

    case ESP_GATTC_CONGEST_EVT: {
        Serial.println("#ESP_GATTC_CONGEST_EVT");
    } break;

    case ESP_GATTC_CANCEL_OPEN_EVT: {
        Serial.println("#ESP_GATTC_CANCEL_OPEN_EVT");
    } break;

    case ESP_GATTC_CFG_MTU_EVT: {
        Serial.println("#ESP_GATTC_CFG_MTU_EVT ");
    } break;

    case ESP_GATTC_SEARCH_RES_EVT: {
        Serial.print("#ESP_GATTC_SEARCH_RES_EVT) srvc_id=0x");
    } break;

    case ESP_GATTC_SEARCH_CMPL_EVT: {
        Serial.println("#ESP_GATTC_SEARCH_CMPL_EVT ");
    } break;

    case ESP_GATTC_REG_FOR_NOTIFY_EVT: {
          Serial.println("#ESP_GATTC_REG_FOR_NOTIFY_EVT ??");
    } break;

    case ESP_GATTC_NOTIFY_EVT: {
      BTAddr a(p_data->notify.remote_bda);
//      a.println("#ESP_GATTC_NOTIFY_EVT ");
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
      pBLE->onWritten(p_data->write.status == ESP_GATT_OK);
    } break;

    case ESP_GATTC_DISCONNECT_EVT: {
      BTAddr a(p_data->disconnect.remote_bda);
//      a.println("#ESP_GATTC_DISCONNECT_EVT ");
      eState state = pBLE->getConnState(a);
      if (state == connected) { // only send disconnect, if connected before
        pBLE->onDisconnected(a);
      }
  } break;

    default:
      Serial.println("#gattc_profile_event_handler:default)");
        break;
    }
}

void BleBase::esp_gap_cb(esp_gap_ble_cb_event_t event, esp_ble_gap_cb_param_t *param)
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
    } break;

    case ESP_GAP_BLE_ADV_STOP_COMPLETE_EVT: {
          Serial.println("#ESP_GAP_BLE_ADV_STOP_COMPLETE_EVT");
    } break;

    case ESP_GAP_BLE_UPDATE_CONN_PARAMS_EVT: {
          Serial.println("#ESP_GAP_BLE_UPDATE_CONN_PARAMS_EVT");
    } break;

    default:
        Serial.println("default)");
        break;
    }
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
    ret = esp_ble_gattc_register_callback(gattc_profile_event_handler);
    if(ret){
      Serial.println("-initBLE6");
      return false;
    }

    ret = esp_ble_gattc_app_register(APP_ID);
    if (ret){
      Serial.println("-initBLE7");
    }
  
    esp_err_t local_mtu_ret = esp_ble_gatt_set_local_mtu(50);
    if (local_mtu_ret){
      Serial.println("-initBLE8");
    }

    Serial.println("BleBase::init: ok");
    delay(200);
    return true;
}


bool BleBase::write(const BTAddr &addr, uint16_t handle, const uint8_t* data, uint8_t len, bool response) {

    p("BleBase::write(handle=0x%x, len=%d) ", handle, len);
    uint16_t connId = getConnId(addr);

    esp_err_t errRc = ::esp_ble_gattc_write_char(
        pBLE->a_gattc_if,
        connId,
        handle,
        len,
        (uint8_t*)data,
        response ? ESP_GATT_WRITE_TYPE_RSP : ESP_GATT_WRITE_TYPE_NO_RSP,
        ESP_GATT_AUTH_REQ_NONE
    );

    if (errRc != ESP_OK) {
        p("failed\n");
        return false;
    } else {
        p("ok\n");
        return true;
    }
}

bool BleBase::connect(const BTAddr& addr) {
    addr.println("BleBase::connect()");
    esp_err_t errRc = esp_ble_gattc_open(a_gattc_if, (uint8_t*)addr.addr, true);
//    Serial.println(errRc == ESP_OK ? ") ok" : ") failed");
    if(errRc == ESP_OK) {
        onConnecting(addr);
        return true;
    } else {
        onConnectFailed(addr);
        return false;
    }
}


bool BleBase::registerNotify(const BTAddr &addr, uint16_t handle) {
    Serial.print("BleBase::registerNotify() handle=0x");
    Serial.println(handle, HEX);
    esp_ble_gattc_register_for_notify (pBLE->a_gattc_if, (uint8_t*)addr.addr, handle);
    return true;
}


BleBase::BleBase() {

  connStateMutex = xSemaphoreCreateMutex();
  memset(&connState, 0, sizeof(connState));
}


int BleBase::getConnIndex(const BTAddr &addr) {
  
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


void BleBase::setConnState(const BTAddr &addr, eState state, uint16_t connId) {

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

  p("BleBase::setConnState(%s", eState2Str(state));
  if (connId != CONNID_INVALID) {
    p(", connId=0x%x", connId);
  }
  p(", i=%d)", i);
  addr.println("");
}


eState BleBase::getConnState(const BTAddr &addr) {

  xSemaphoreTake(connStateMutex, MUTEX_MAX_DELAY);
  eState ret = disconnected;
  int i = getConnIndex(addr);
  if (i >= 0) {
    ret = connState[i].state;
  }
  xSemaphoreGive(connStateMutex);
  return ret;
}


uint16_t BleBase::getConnId(const BTAddr &addr) {
  
  xSemaphoreTake(connStateMutex, MUTEX_MAX_DELAY);
  int i = getConnIndex(addr);
  uint16_t connId = 0;
  if (i > 0) {
    connId = connState[i].connId;
  }
  xSemaphoreGive(connStateMutex);
  return connId;
}


bool BleBase::canConnect() {
  //return gatt_find_i_tcb_free() != 0xFF;
  return countStates(eState::connecting) == 0;
}

void BleBase::onConnectFailed(const BTAddr &addr) {
    addr.println("BleBase::onConnectFailed()");
    setConnState(addr, failed, CONNID_INVALID);
    resetConnState(addr);
}

void BleBase::onDisconnected(const BTAddr &addr) {
    addr.println("BleBase::disconnected()");
    setConnState(addr, disconnected, CONNID_INVALID);
    resetConnState(addr);
}

void BleBase::onConnected(const BTAddr &addr, uint16_t connId) {
    addr.println("BleBase::onConnected()");
    setConnState(addr, connected, connId);
}

void BleBase::onConnecting(const BTAddr &addr) {
    addr.println("BleBase::onConnecting()");
    setConnState(addr, connecting, CONNID_INVALID);
}

void BleBase::onReceiveData(const BTAddr &addr, const uint8_t* pData, uint8_t len) {
    addr.println("BleBase::onReceiveData()");
}

void BleBase::onServiceFound() {
    p("BleBase::onServiceFound()");
}

void BleBase::onWritten(bool success) {
//    Serial.print("BleBase::onWritten() ");
//    Serial.println(success ? "ok" : "failed");
}

void BleBase::resetConnState(const BTAddr &addr) {
  xSemaphoreTake(connStateMutex, MUTEX_MAX_DELAY);
  for (int i=0; i<MAX_CONNECTIONS; i++) {
    if(addr.isSame(connState[i].addr)) {
      memset(&connState[i], 0, sizeof(tConnState));
    }
  }  
  xSemaphoreGive(connStateMutex);
}

void BleBase::disconnect(const BTAddr &addr) {
    esp_ble_gap_disconnect((uint8_t*)addr.addr);
}


uint8_t BleBase::countStates(eState state) {

    uint8_t n = 0;
    xSemaphoreTake(connStateMutex, MUTEX_MAX_DELAY);
    for (int i=0; i<MAX_CONNECTIONS; i++) {
        tConnState &st = connState[i];
        if ((st.state == state) && (st.addr.isValid())) {
            n++;
        }
    }  
    xSemaphoreGive(connStateMutex);
    return n;
}
