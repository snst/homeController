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
    AutoLock l(pBLE->mutexBT);
    esp_ble_gattc_cb_param_t *p_data = (esp_ble_gattc_cb_param_t *)param;

    switch (event) {
    case ESP_GATTC_REG_EVT: {
        p(20, "#ESP_GATTC_REG_EVT app_id=0x%x\n", param->reg.app_id);

        if (param->reg.status == ESP_GATT_OK) {
            pBLE->a_gattc_if = gattc_if;
        } else {
            p(20, "#ESP_GATTC_REG_EVT FAILED\n");
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
        p(20, "#ESP_GATTC_QUEUE_FULL_EVT\n");
    } break;

    case ESP_GATTC_CONGEST_EVT: {
        p(20, "#ESP_GATTC_CONGEST_EVT\n");
    } break;

    case ESP_GATTC_CANCEL_OPEN_EVT: {
        p(20, "#ESP_GATTC_CANCEL_OPEN_EVT\n");
    } break;

    case ESP_GATTC_CFG_MTU_EVT: {
        p(20, "#ESP_GATTC_CFG_MTU_EVT \n");
    } break;

    case ESP_GATTC_SEARCH_RES_EVT: {
        p(20, "#ESP_GATTC_SEARCH_RES_EVT) srvc_id=0x\n");
    } break;

    case ESP_GATTC_SEARCH_CMPL_EVT: {
        p(20, "#ESP_GATTC_SEARCH_CMPL_EVT \n");
    } break;

    case ESP_GATTC_REG_FOR_NOTIFY_EVT: {
          p(20, "#ESP_GATTC_REG_FOR_NOTIFY_EVT ??\n");
    } break;

    case ESP_GATTC_NOTIFY_EVT: {
      BTAddr a(p_data->notify.remote_bda);
//      a.println("#ESP_GATTC_NOTIFY_EVT ");
      if (p_data->notify.is_notify) {
        pBLE->onReceiveData(a, p_data->notify.value, p_data->notify.value_len);
      }
    } break;

    case ESP_GATTC_WRITE_DESCR_EVT: {
          p(20, "#ESP_GATTC_WRITE_DESCR_EVT ??\n");
    } break;

    case ESP_GATTC_SRVC_CHG_EVT: {
        p(20, "#ESP_GATTC_SRVC_CHG_EVT ??\n");
    } break;
    case ESP_GATTC_WRITE_CHAR_EVT: {
      //Serial.print("ESP_GATTC_WRITE_CHAR_EVT) ");
      pBLE->onWritten(p_data->write.status == ESP_GATT_OK);
    } break;

    case ESP_GATTC_DISCONNECT_EVT: {
      BTAddr a(p_data->disconnect.remote_bda);
//      a.println("#ESP_GATTC_DISCONNECT_EVT ");
      eState state = pBLE->connState.get(a);
      if (state == connected) { // only send disconnect, if connected before
        pBLE->onDisconnected(a);
      }
  } break;

    default:
      p(20, "#gattc_profile_event_handler:default)\n");
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
        p(20, "#ESP_GAP_BLE_SCAN_PARAM_SET_COMPLETE_EVT\n");
        //the unit of the duration is second
        uint32_t duration = 30;
        esp_ble_gap_start_scanning(duration);
    } break;

    case ESP_GAP_BLE_SCAN_START_COMPLETE_EVT: {
        p(20, "#ESP_GAP_BLE_SCAN_START_COMPLETE_EVT ");
        //scan start complete event to indicate scan start successfully or failed
        if (param->scan_start_cmpl.status != ESP_BT_STATUS_SUCCESS) {
            p(20, "failed err=0x%x\n", param->scan_start_cmpl.status);
        } else {
            p(20, "ok\n");
        }
    } break;

    case ESP_GAP_BLE_SCAN_RESULT_EVT: {
        p(20, "#ESP_GAP_BLE_SCAN_RESULT_EVT");
        esp_ble_gap_cb_param_t *scan_result = (esp_ble_gap_cb_param_t *)param;
        switch (scan_result->scan_rst.search_evt) {
            case ESP_GAP_SEARCH_INQ_RES_EVT: {
                p(20, "##ESP_GAP_SEARCH_INQ_RES_EVT)\n");
            } break;
            case ESP_GAP_SEARCH_INQ_CMPL_EVT: {
                p(20, "##ESP_GAP_SEARCH_INQ_CMPL_EVT)\n");
            } break;
            
            default: {
                p(20, "##default)\n");
            } break;
        }
    } break;

    case ESP_GAP_BLE_SCAN_STOP_COMPLETE_EVT: {
          p(20, "#ESP_GAP_BLE_SCAN_STOP_COMPLETE_EVT\n");
    } break;

    case ESP_GAP_BLE_ADV_STOP_COMPLETE_EVT: {
          p(20, "#ESP_GAP_BLE_ADV_STOP_COMPLETE_EVT\n");
    } break;

    case ESP_GAP_BLE_UPDATE_CONN_PARAMS_EVT: {
          p(20, "#ESP_GAP_BLE_UPDATE_CONN_PARAMS_EVT\n");
    } break;

    default:
        p(20, "default)\n");
        break;
    }
}


bool BleBase::init()
{
  //  p(20, "+BleBase::init");
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
      p(20, "-initBLE1\n");
      return false;
    }

    ret = esp_bt_controller_enable(ESP_BT_MODE_BLE);
    if (ret) {
      p(20, "-initBLE2\n");
      return false;
    }

    ret = esp_bluedroid_init();
    if (ret) {
      p(20, "-initBLE3\n");
      return false;
    }

    ret = esp_bluedroid_enable();
    if (ret) {
       p(20, "-initBLE4\n");
       return false;
    }

    //register the  callback function to the gap module
    ret = esp_ble_gap_register_callback(esp_gap_cb);
    if (ret){
      p(20, "-initBLE5\n");
      return false;
    }

    //register the callback function to the gattc module
    ret = esp_ble_gattc_register_callback(gattc_profile_event_handler);
    if(ret){
      p(20, "-initBLE6\n");
      return false;
    }

    ret = esp_ble_gattc_app_register(APP_ID);
    if (ret){
      p(20, "-initBLE7\n");
    }
  
    esp_err_t local_mtu_ret = esp_ble_gatt_set_local_mtu(50);
    if (local_mtu_ret){
      p(20, "-initBLE8\n");
    }

    p(20, "BleBase::init: ok\n");
    sleep(200);
    return true;
}


bool BleBase::write(const BTAddr &addr, uint16_t handle, const uint8_t* data, uint8_t len, bool response) {

    p(20, "BleBase::write(handle=0x%x, len=%d) ", handle, len);
    uint16_t connId = connState.getId(addr);

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
        p(20, "failed\n");
        return false;
    } else {
        p(20, "ok\n");
        return true;
    }
}

bool BleBase::connect(const BTAddr& addr) {
    addr.println(20, "BleBase::connect()");
    onConnecting(addr);
    esp_err_t errRc = esp_ble_gattc_open(a_gattc_if, (uint8_t*)addr.addr, true);
    return (errRc == ESP_OK);
}


bool BleBase::registerNotify(const BTAddr &addr, uint16_t handle) {
    p(20, "BleBase::registerNotify() handle=0x%x\n", handle);
    return ESP_OK == esp_ble_gattc_register_for_notify(pBLE->a_gattc_if, (uint8_t*)addr.addr, handle);
}

bool BleBase::unregisterNotify(const BTAddr &addr, uint16_t handle) {
    p(20, "BleBase::unregisterNotify() handle=0x%x\n", handle);
    return ESP_OK == esp_ble_gattc_unregister_for_notify(pBLE->a_gattc_if, (uint8_t*)addr.addr, handle);
}

BleBase::BleBase() {
}


bool BleBase::isConnecting() {
  //return gatt_find_i_tcb_free() != 0xFF;
  return connState.count(eState::connecting) != 0;
}

bool BleBase::hasFreeConnections() {
  return gatt_find_i_tcb_free() != 0xFF;
  //uint8_t nConnecting = connState.count(eState::connecting);
  //uint8_t nConnected = connState.count(eState::connected);
//  p("hasFreeConnections(%d,%d)\n", nConnecting, nConnected);
  //return (nConnecting + nConnected) < MAX_OPEN_CONNECTIONS;
}

void BleBase::onConnectFailed(const BTAddr &addr) {
    addr.println(20, "BleBase::onConnectFailed()");
    connState.reset(addr);
}

void BleBase::onDisconnected(const BTAddr &addr) {
    addr.println(20, "BleBase::disconnected()");
    connState.reset(addr);
}

void BleBase::onConnected(const BTAddr &addr, uint16_t connId) {
    addr.println(20, "BleBase::onConnected()");
    connState.set(addr, connected, connId);
}

void BleBase::onConnecting(const BTAddr &addr) {
    addr.println(20, "BleBase::onConnecting()");
    connState.set(addr, connecting, CONNID_INVALID);
}

void BleBase::onReceiveData(const BTAddr &addr, const uint8_t* pData, uint8_t len) {
    addr.println(20, "BleBase::onReceiveData()");
}

void BleBase::onServiceFound() {
    p(20, "BleBase::onServiceFound()\n");
}

void BleBase::onWritten(bool success) {
//    Serial.print("BleBase::onWritten() ");
//    Serial.println(success ? "ok" : "failed");
}


void BleBase::disconnect(const BTAddr &addr) {
    esp_ble_gap_disconnect((uint8_t*)addr.addr);
}

void BleBase::closeOldestConnection() {
    BTAddr addr;
    if (connState.getOldestOpenConn(addr)) {
        addr.println(20, "closeOldestConnection");
        disconnect(addr);
    }
}