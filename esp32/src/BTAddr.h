#ifndef __BTADDR_H__
#define __BTADDR_H__

#include <Arduino.h>
#include <stdio.h>
#include <string.h>
#include <esp_bt_defs.h>

#define BT_ADDR_SIZE (sizeof(esp_bd_addr_t))

class BTAddr
{
  public:
    esp_bd_addr_t addr;
    
    BTAddr() {
      reset();
    }
    
    BTAddr(const uint8_t *_addr) {
      setAddr(_addr);
    }

    BTAddr& operator= (const BTAddr &src) {
      memcpy(addr, src.addr, sizeof(esp_bd_addr_t));
      return *this;
    }

    void reset() {
      memset(&addr, 0, sizeof(esp_bd_addr_t)); 
    }

    bool isValid() const {
      uint8_t* a = (uint8_t*) addr;
      bool ret = !(a[0]==0 && a[1]==0 && a[2]==0 && a[3]==0 && a[4]==0 && a[5]==0);
      return ret;
    }

    void print(const char *txt) const {
      Serial.print(txt);
      for(int i=0; i<sizeof(esp_bd_addr_t); i++) {
        Serial.print(":");
        Serial.print(((uint8_t*)addr)[i], HEX);
      }
    }

    void println(const char *txt) const {
      print(txt);
      Serial.println("");
    }

    void setStrAddr(const char *strAddr) {
      uint8_t* a = (uint8_t*) addr;
      sscanf(strAddr, "%x:%x:%x:%x:%x:%x", &a[0], &a[1], &a[2], &a[3], &a[4], &a[5]);
    }

    void setAddr(const uint8_t *_addr) {
      memcpy((uint8_t*)addr, _addr, sizeof(esp_bd_addr_t));
    }

    bool isSame(BTAddr &bleAddr) const {
      return !memcmp((uint8_t*)addr, (uint8_t*)bleAddr.addr, sizeof(esp_bd_addr_t));
    }

    void setAddr(BTAddr &bleAddr) {
      memcpy((uint8_t*)addr, bleAddr.addr, sizeof(esp_bd_addr_t));
    }
};


#endif
