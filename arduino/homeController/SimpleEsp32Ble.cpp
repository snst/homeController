// Copyright 2017 Stefan Schmidt

#include "SimpleEsp32Ble.h"

#define GATTC_TAG "GATTC_DEMO"
//#define REMOTE_SERVICE_UUID        0x7046
//#define REMOTE_NOTIFY_CHAR_UUID_CMD    0xea09
//#define REMOTE_NOTIFY_CHAR_UUID_PERLY    0xeb2a

#define PROFILE_NUM      1
#define INVALID_HANDLE   0

//static esp_gattc_char_elem_t *char_elem_result   = NULL;
//static esp_gattc_descr_elem_t *descr_elem_result = NULL;

static void esp_gap_cb(esp_gap_ble_cb_event_t event, esp_ble_gap_cb_param_t *param);
static void esp_gattc_cb(esp_gattc_cb_event_t event, esp_gatt_if_t gattc_if, esp_ble_gattc_cb_param_t *param);
static void gattc_profile_event_handler(esp_gattc_cb_event_t event, esp_gatt_if_t gattc_if, esp_ble_gattc_cb_param_t *param);

static SimpleBLE* pBLE = nullptr;

/*
static esp_bt_uuid_t remote_filter_service_uuid = {
    .len = ESP_UUID_LEN_16,
    .uuid = {.uuid16 = REMOTE_SERVICE_UUID,},
};

static esp_bt_uuid_t remote_filter_char_uuid = {
    .len = ESP_UUID_LEN_16,
    .uuid = {.uuid16 = REMOTE_NOTIFY_CHAR_UUID_CMD,},
};

static esp_bt_uuid_t notify_descr_uuid = {
    .len = ESP_UUID_LEN_16,
    .uuid = {.uuid16 = ESP_GATT_UUID_CHAR_CLIENT_CONFIG,},
};

static esp_ble_scan_params_t ble_scan_params = {
    .scan_type              = BLE_SCAN_TYPE_ACTIVE,
    .own_addr_type          = BLE_ADDR_TYPE_PUBLIC,
    .scan_filter_policy     = BLE_SCAN_FILTER_ALLOW_ALL,
    .scan_interval          = 0x50,
    .scan_window            = 0x30
};*/

void printAddr(char* c, uint8_t* d) {
 /* Serial.print("+Addr-");
  Serial.println(c);
  Serial.println(sizeof(esp_bd_addr_t));
  for(int i=0;i<sizeof(esp_bd_addr_t);i++)
    Serial.println(d[i], HEX);
  Serial.println("---");*/
}

static void gattc_profile_event_handler(esp_gattc_cb_event_t event, esp_gatt_if_t gattc_if, esp_ble_gattc_cb_param_t *param)
{
    esp_ble_gattc_cb_param_t *p_data = (esp_ble_gattc_cb_param_t *)param;
    Serial.print("SBLE::gattc_profile_event_handler(");

    switch (event) {
    case ESP_GATTC_REG_EVT: {
        Serial.println("ESP_GATTC_REG_EVT)");
        //          doConnect = true;
        /* esp_err_t scan_ret = esp_ble_gap_set_scan_params(&ble_scan_params);
        if (scan_ret){
        ESP_LOGE(GATTC_TAG, "set scan params error, error code = %x", scan_ret);
        } */
    } break;

    case ESP_GATTC_CONNECT_EVT: {
        Serial.print("ESP_GATTC_CONNECT_EVT) ");
        pBLE->gattcProfile[APP_ID].conn_id = p_data->connect.conn_id;
        Serial.println(p_data->connect.conn_id);
        memcpy(pBLE->gattcProfile[APP_ID].remote_bda, p_data->connect.remote_bda, sizeof(esp_bd_addr_t));
//    printAddr("c", p_data->connect.remote_bda);
        esp_err_t mtu_ret = esp_ble_gattc_send_mtu_req (gattc_if, p_data->connect.conn_id);
        if(mtu_ret) {
  //          ESP_LOGE(GATTC_TAG, "config MTU error, error code = %x", mtu_ret);
            Serial.println("MTU error");
        } else {
            Serial.println("ok");
        }
    } break;

    case ESP_GATTC_OPEN_EVT: {
        Serial.print("ESP_GATTC_OPEN_EVT) ");

        if (param->open.status != ESP_GATT_OK){
       //     ESP_LOGE(GATTC_TAG, "open failed, status %d", p_data->open.status);
            Serial.println("failed");
        } else {
            Serial.println("ok");
        }
    } break;

    case ESP_GATTC_CFG_MTU_EVT: {
        Serial.print("ESP_GATTC_CFG_MTU_EVT) ");
        if (param->cfg_mtu.status != ESP_GATT_OK){
//            ESP_LOGE(GATTC_TAG,"config mtu failed, error status = %x", param->cfg_mtu.status);
            Serial.println("failed");
        } else {
            Serial.println("ok");
//          ESP_LOGI(GATTC_TAG, "ESP_GATTC_CFG_MTU_EVT, Status %d, MTU %d, conn_id %d", param->cfg_mtu.status, param->cfg_mtu.mtu, param->cfg_mtu.conn_id);
//          esp_ble_gattc_search_service(gattc_if, param->cfg_mtu.conn_id, &remote_filter_service_uuid);

            pBLE->setState(SimpleBLE::connected);
            pBLE->onConnected();
            // search service
            //Serial.println("call esp_ble_gattc_search_service");
            //esp_ble_gattc_search_service(gattc_if, param->cfg_mtu.conn_id, nullptr);
        }        
    } break;

    case ESP_GATTC_SEARCH_RES_EVT: {
        Serial.print("ESP_GATTC_SEARCH_RES_EVT) srvc_id=0x");
        /*
        esp_gatt_srvc_id_t *srvc_id =(esp_gatt_srvc_id_t *)&p_data->search_res.srvc_id;
       // Serial.println(srvc_id->id.uuid.len);
        Serial.println(srvc_id->id.uuid.uuid.uuid16, HEX);
//        if (srvc_id->id.uuid.len == ESP_UUID_LEN_16 && srvc_id->id.uuid.uuid.uuid16 == REMOTE_SERVICE_UUID) {
        if ( srvc_id->id.uuid.uuid.uuid16 == REMOTE_SERVICE_UUID) {
            Serial.println("service found");
            get_server = true;
            pBLE->gattcProfile[APP_ID].service_start_handle = p_data->search_res.start_handle;
            pBLE->gattcProfile[APP_ID].service_end_handle = p_data->search_res.end_handle;
    //        ESP_LOGI(GATTC_TAG, "UUID16: %x", srvc_id->id.uuid.uuid.uuid16);
        }*/
    } break;

    case ESP_GATTC_SEARCH_CMPL_EVT: {
        Serial.print("ESP_GATTC_SEARCH_CMPL_EVT) ");
        if (p_data->search_cmpl.status != ESP_GATT_OK) {
//            ESP_LOGE(GATTC_TAG, "search service failed, error status = %x", p_data->search_cmpl.status);
            Serial.println("failed");
        } else {
            Serial.println("ok");
            pBLE->setState(SimpleBLE::ready);
            pBLE->onServiceFound();
        }

/*        if (get_server){
            uint16_t count = 0;
            esp_gatt_status_t status = esp_ble_gattc_get_attr_count( gattc_if,
                                                                     p_data->search_cmpl.conn_id,
                                                                     ESP_GATT_DB_CHARACTERISTIC,
                                                                     pBLE->gattcProfile[APP_ID].service_start_handle,
                                                                     pBLE->gattcProfile[APP_ID].service_end_handle,
                                                                     INVALID_HANDLE,
                                                                     &count);
            if (status != ESP_GATT_OK){
              Serial.println("esp_ble_gattc_get_attr_count error");
            }

            if (count > 0){
                char_elem_result = (esp_gattc_char_elem_t *)malloc(sizeof(esp_gattc_char_elem_t) * count);
                if (!char_elem_result){
//                    ESP_LOGE(GATTC_TAG, "gattc no mem");
                }else{
                  Serial.println("esp_ble_gattc_get_char_by_uuid");

                    
                    uint16_t offset = 0;
                      esp_gattc_char_elem_t result;
                      while(1) {
                        uint16_t count = 1;
                        esp_gatt_status_t status = ::esp_ble_gattc_get_all_char(
                          gattc_if,
                          p_data->search_cmpl.conn_id,
                          pBLE->gattcProfile[APP_ID].service_start_handle,
                          pBLE->gattcProfile[APP_ID].service_end_handle,
                          &result,
                          &count,
                          offset
                        );
                    
                        if (status == ESP_GATT_INVALID_OFFSET) {   // We have reached the end of the entries.
                          Serial.println("end while");
                          break;
                        }
                    
                        if (status != ESP_GATT_OK) {   // If we got an error, end.
                          Serial.println("esp_ble_gattc_get_all_char:");
                          break;
                        }
                    
                        if (count == 0) {   // If we failed to get any new records, end.
                          Serial.println("end while2");
                          break;
                        }
Serial.println("found char");
                    Serial.println(result.char_handle, HEX);
Serial.println(result.uuid.uuid.uuid16, HEX);
offset++; 

//                        ESP_LOGD(LOG_TAG, "Found a characteristic: Handle: %d, UUID: %s", result.char_handle, BLEUUID(result.uuid).toString().c_str());
                      }

esp_ble_gattc_register_for_notify (gattc_if, pBLE->gattcProfile[APP_ID].remote_bda, 0x421);


connected = true;

pBLE->state = SimpleBLE::connected;
pBLE->onConnected();


                  // COMMENT ++
                    status = esp_ble_gattc_get_char_by_uuid( gattc_if,
                                                             p_data->search_cmpl.conn_id,
                                                             pBLE->gattcProfile[APP_ID].service_start_handle,
                                                             pBLE->gattcProfile[APP_ID].service_end_handle,
                                                             remote_filter_char_uuid,
                                                             char_elem_result,
                                                             &count);
                    if (status != ESP_GATT_OK){
                      Serial.println("esp_ble_gattc_get_char_by_uuid error");
                    }
                      Serial.println("esp_ble_gattc_get_char_by_uuid OK!!");

                    //  Every service have only one char in our 'ESP_GATTS_DEMO' demo, so we used first 'char_elem_result' 
                    if (count > 0 && (char_elem_result[0].properties & ESP_GATT_CHAR_PROP_BIT_NOTIFY)){
                        pBLE->gattcProfile[APP_ID].char_handle = char_elem_result[0].char_handle;
                        esp_ble_gattc_register_for_notify (gattc_if, pBLE->gattcProfile[APP_ID].remote_bda, char_elem_result[0].char_handle);
                    }
                    // COMMENT --
                }
                /// free char_elem_result 
                free(char_elem_result);
            }else{
            //    ESP_LOGE(GATTC_TAG, "no char found");
            }
        }
        */
        
    } break;

    case ESP_GATTC_REG_FOR_NOTIFY_EVT: {
          Serial.print("ESP_GATTC_REG_FOR_NOTIFY_EVT) ??");
          /*
        if (p_data->reg_for_notify.status != ESP_GATT_OK) {
           Serial.println("failed: status");
    //        ESP_LOGE(GATTC_TAG, "REG FOR NOTIFY failed: error status = %d", p_data->reg_for_notify.status);
        } else {
          
            uint16_t count = 0;
            uint16_t notify_en = 1;
            esp_gatt_status_t ret_status = esp_ble_gattc_get_attr_count( gattc_if,
                                                                         pBLE->gattcProfile[APP_ID].conn_id,
                                                                         ESP_GATT_DB_DESCRIPTOR,
                                                                         pBLE->gattcProfile[APP_ID].service_start_handle,
                                                                         pBLE->gattcProfile[APP_ID].service_end_handle,
                                                                         pBLE->gattcProfile[APP_ID].char_handle,
                                                                         &count);
            if (ret_status != ESP_GATT_OK) {
                Serial.println("failed: esp_ble_gattc_get_attr_count");
            }

            if (count > 0) {
                descr_elem_result = (esp_gattc_descr_elem_t*)malloc(sizeof(esp_gattc_descr_elem_t) * count);
                if (!descr_elem_result) {
                    Serial.println("failed: malloc");
                } else {
                    ret_status = esp_ble_gattc_get_descr_by_char_handle( gattc_if,
                                                                         pBLE->gattcProfile[APP_ID].conn_id,
                                                                         p_data->reg_for_notify.handle,
                                                                         notify_descr_uuid,
                                                                         descr_elem_result,
                                                                         &count);
                    if (ret_status != ESP_GATT_OK) {
                        Serial.println("failed: esp_ble_gattc_get_descr_by_char_handle error");
                    }

                    // Erery char have only one descriptor in our 'ESP_GATTS_DEMO' demo, so we used first 'descr_elem_result' 
                    if (count > 0 && descr_elem_result[0].uuid.len == ESP_UUID_LEN_16 && descr_elem_result[0].uuid.uuid.uuid16 == ESP_GATT_UUID_CHAR_CLIENT_CONFIG){
                        ret_status = (esp_gatt_status_t) esp_ble_gattc_write_char_descr( gattc_if, // stsc?
                                                                     pBLE->gattcProfile[APP_ID].conn_id,
                                                                     descr_elem_result[0].handle,
                                                                     sizeof(notify_en),
                                                                     (uint8_t *)&notify_en,
                                                                     ESP_GATT_WRITE_TYPE_RSP,
                                                                     ESP_GATT_AUTH_REQ_NONE);
                    }

                    if (ret_status != ESP_GATT_OK) {
                        Serial.println("failed: esp_ble_gattc_write_char_descr");
                    }

                    // free descr_elem_result 
                    free(descr_elem_result);
                }
            }
            else {
                Serial.println("failed: decsr not found");
            }
        } */
    } break;

    case ESP_GATTC_NOTIFY_EVT: {
        Serial.print("ESP_GATTC_NOTIFY_EVT) ");
        if (p_data->notify.is_notify) {
          Serial.println("notify");
          pBLE->onReceiveNotify(p_data->notify.value, p_data->notify.value_len);
        } else {
          Serial.println("indicate ???");
          pBLE->onReceiveIndicate(p_data->notify.value, p_data->notify.value_len);
        }
        //Serial.println(p_data->notify.value_len, HEX);
        //for(int i=0;i<p_data->notify.value_len; i++)
        //Serial.println(p_data->notify.value[i],  HEX);
    } break;

    case ESP_GATTC_WRITE_DESCR_EVT: {
          Serial.print("ESP_GATTC_WRITE_DESCR_EVT) ??");
          /*
        if (p_data->write.status != ESP_GATT_OK) {
            Serial.println("failed");
       //     ESP_LOGE(GATTC_TAG, "write descr failed, error status = %x", p_data->write.status);
        } else {
            Serial.println("ok");
            uint8_t write_char_data[35];
            for (int i = 0; i < sizeof(write_char_data); ++i)
            {
                write_char_data[i] = i % 256;
            }
            esp_ble_gattc_write_char( gattc_if,
                                    pBLE->gattcProfile[APP_ID].conn_id,
                                    pBLE->gattcProfile[APP_ID].char_handle,
                                    sizeof(write_char_data),
                                    write_char_data,
                                    ESP_GATT_WRITE_TYPE_RSP,
                                    ESP_GATT_AUTH_REQ_NONE);
        }*/
    } break;

    case ESP_GATTC_SRVC_CHG_EVT: {
        Serial.println("ESP_GATTC_SRVC_CHG_EVT) ??");
//        esp_bd_addr_t bda;
//        memcpy(bda, p_data->srvc_chg.remote_bda, sizeof(esp_bd_addr_t));
//        ESP_LOGI(GATTC_TAG, "ESP_GATTC_SRVC_CHG_EVT, bd_addr:");
  //      esp_log_buffer_hex(GATTC_TAG, bda, sizeof(esp_bd_addr_t));
    } break;

    case ESP_GATTC_WRITE_CHAR_EVT: {
      Serial.print("ESP_GATTC_WRITE_CHAR_EVT) ");
    printAddr("w", p_data->connect.remote_bda);
      pBLE->isWriting = false;
      pBLE->onWritten(p_data->write.status == ESP_GATT_OK);
      /*
        Serial.print("ESP_GATTC_WRITE_CHAR_EVT) ");
        if (p_data->write.status == ESP_GATT_OK){
            Serial.println("ok");
        } else {
            Serial.println("failed");
         //   ESP_LOGE(GATTC_TAG, "write char failed, error status = %x", p_data->write.status);
        }*/
    } break;

    case ESP_GATTC_DISCONNECT_EVT: {
        Serial.println("ESP_GATTC_DISCONNECT_EVT)");
//        Serial.println(p_data->disconnect.reason, HEX);
        pBLE->setState(SimpleBLE::disconnected);
        pBLE->onDisconnected();
    } break;

    default:
      Serial.println("default)");
        break;
    }
}

static void esp_gap_cb(esp_gap_ble_cb_event_t event, esp_ble_gap_cb_param_t *param)
{
    uint8_t *adv_name = NULL;
    uint8_t adv_name_len = 0;
    Serial.print("SBLE::esp_gap_cp(");

    switch (event) {
    case ESP_GAP_BLE_SCAN_PARAM_SET_COMPLETE_EVT: {
        Serial.println("ESP_GAP_BLE_SCAN_PARAM_SET_COMPLETE_EVT)");
        //the unit of the duration is second
        uint32_t duration = 30;
        esp_ble_gap_start_scanning(duration);
    } break;

    case ESP_GAP_BLE_SCAN_START_COMPLETE_EVT: {
        Serial.print("ESP_GAP_BLE_SCAN_START_COMPLETE_EVT) ");
        //scan start complete event to indicate scan start successfully or failed
        if (param->scan_start_cmpl.status != ESP_BT_STATUS_SUCCESS) {
            Serial.print("failed err=0x");
            Serial.println(param->scan_start_cmpl.status, HEX);
        } else {
            Serial.println("ok");
        }
    } break;

    case ESP_GAP_BLE_SCAN_RESULT_EVT: {
        Serial.print("ESP_GAP_BLE_SCAN_RESULT_EVT, ");
        esp_ble_gap_cb_param_t *scan_result = (esp_ble_gap_cb_param_t *)param;
        switch (scan_result->scan_rst.search_evt) {
            case ESP_GAP_SEARCH_INQ_RES_EVT: {
                Serial.println("ESP_GAP_SEARCH_INQ_RES_EVT)");
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
                Serial.println("ESP_GAP_SEARCH_INQ_CMPL_EVT)");
            } break;
            
            default: {
                Serial.println("default)");

            } break;
        }
    } break;

    case ESP_GAP_BLE_SCAN_STOP_COMPLETE_EVT: {
          Serial.println("ESP_GAP_BLE_SCAN_STOP_COMPLETE_EVT)");
        if (param->scan_stop_cmpl.status != ESP_BT_STATUS_SUCCESS){
  //          ESP_LOGE(GATTC_TAG, "scan stop failed, error status = %x", param->scan_stop_cmpl.status);
            break;
        }
//        ESP_LOGI(GATTC_TAG, "stop scan successfully");
    } break;

    case ESP_GAP_BLE_ADV_STOP_COMPLETE_EVT: {
          Serial.println("ESP_GAP_BLE_ADV_STOP_COMPLETE_EVT)");
        if (param->adv_stop_cmpl.status != ESP_BT_STATUS_SUCCESS){
         //   ESP_LOGE(GATTC_TAG, "adv stop failed, error status = %x", param->adv_stop_cmpl.status);
            break;
        }
//        ESP_LOGI(GATTC_TAG, "stop adv successfully");
    } break;

    case ESP_GAP_BLE_UPDATE_CONN_PARAMS_EVT: {
          Serial.println("ESP_GAP_BLE_UPDATE_CONN_PARAMS_EVT)");
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
    /* If event is register event, store the gattc_if for each profile */
    if (event == ESP_GATTC_REG_EVT) {
        Serial.print("SBLE::esp_gattc_cb(ESP_GATTC_REG_EVT) app_id=0x");
        Serial.print(param->reg.app_id, HEX);

        if (param->reg.status == ESP_GATT_OK) {
            Serial.println(" ok");
            if(param->reg.app_id<MAX_APP) {
                pBLE->gattcProfile[param->reg.app_id].gattc_if = gattc_if;
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
                    gattc_if == pBLE->gattcProfile[idx].gattc_if) {
                if (pBLE->gattcProfile[idx].gattc_cb) {
                    pBLE->gattcProfile[idx].gattc_cb(event, gattc_if, param);
                }
            }
        }
    } while (0);
}

bool SimpleBLE::init()
{
  //  Serial.println("+SimpleBLE::init");
    pBLE = this;

    gattcProfile[APP_ID].gattc_cb = gattc_profile_event_handler;
    gattcProfile[APP_ID].gattc_if = ESP_GATT_IF_NONE;
    
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

    Serial.println("SimpleBLE::init: ok");
    setState(disconnected);

    vTaskDelay(200/portTICK_PERIOD_MS);
    return true;
}

bool SimpleBLE::write(uint16_t handle, uint8_t* data, uint8_t len, bool response) {
    Serial.print("SimpleBLE::write(handle=0x");
    Serial.print(handle, HEX);
    Serial.print(", len=");
    Serial.print(len);

    esp_err_t errRc = ::esp_ble_gattc_write_char(
        pBLE->gattcProfile[APP_ID].gattc_if,
        pBLE->gattcProfile[APP_ID].conn_id,
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

bool SimpleBLE::connect(BLEAddr& addr) {
    esp_err_t errRc = esp_ble_gattc_open(gattcProfile[APP_ID].gattc_if, addr.addr, true);
    //printAddr("C", addr.addr);
    Serial.print("SBLE::esp_ble_gattc_open() ");
    Serial.println(errRc == ESP_OK ? "ok" : "failed");

    if(errRc == ESP_OK) {
        setState(connecting);
        return true;
    } else {
      setState(disconnected);
      return false;
    }
}

void SimpleBLE::disconnect() {
  esp_ble_gattc_close(gattcProfile[APP_ID].gattc_if, gattcProfile[APP_ID].conn_id);
  setState(disconnecting);  
}


bool SimpleBLE::registerNotify(uint16_t handle) {
    Serial.print("SimpleBLE::registerNotify() handle=0x");
    Serial.println(handle, HEX);
    esp_ble_gattc_register_for_notify (pBLE->gattcProfile[APP_ID].gattc_if, pBLE->gattcProfile[APP_ID].remote_bda, handle);
    return true;
}


SimpleBLE::SimpleBLE() 
: state(deinit), isWriting(false) {
 //   Serial.println("SimpleBLE()");
}
