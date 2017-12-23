#ifndef __BTADDR_H__
#define __BTADDR_H__

#include <esp_bt_defs.h>

#define BT_ADDR_SIZE (sizeof(esp_bd_addr_t))

class BTAddr
{
  public:
    esp_bd_addr_t addr;
    
    BTAddr() {
      memset(&addr, 0, sizeof(esp_bd_addr_t)); 
    }
    
    BTAddr(uint8_t* _addr) {
      setAddr(_addr);
    }

    BTAddr(char* strAddr) {
      uint8_t* a = (uint8_t*) addr;
      sscanf(strAddr, "%x:%x:%x:%x:%x:%x", &a[0], &a[1], &a[2], &a[3], &a[4], &a[5]);
    }

//    BTAddr(esp_bd_addr_t & _addr) {
//      memcpy(addr, _addr, sizeof(esp_bd_addr_t));
//    }

    BTAddr& operator= (const BTAddr& src) {
      //Serial.println("BtAddr Copy");
      memcpy(addr, src.addr, sizeof(esp_bd_addr_t));
      return *this;
    }

/*
    uint64_t toUint64() {
      uint8_t* a = (uint8_t*) addr;
      uint64_t ret = a[0] | (a[1]<<8) | (a[2]<<16) | (a[3]<<24) | (a[4]<<32) | (a[5]<<40);
//      Serial.print("BT65: ");
//      Serial.println((uint32_t)  (ret&0xFFFFFFFF));
      return ret;
    }
*/
    bool isValid() {
      uint8_t* a = (uint8_t*) addr;
//      print("isValid ", true);
      bool ret = !(a[0]==0 && a[1]==0 && a[2]==0 && a[3]==0 && a[4]==0 && a[5]==0);
  //    Serial.println(ret);
      return ret;
    }

    void print(char* txt, bool linebreak) {
      Serial.print(txt);
      for(int i=0; i<sizeof(esp_bd_addr_t); i++) {
        Serial.print(":");
        Serial.print(((uint8_t*)addr)[i], HEX);
      }
      if(linebreak) {
        Serial.println("");
      }
    }

    void setAddr(uint8_t* _addr) {
      memcpy((uint8_t*)addr, _addr, sizeof(esp_bd_addr_t));
    }

    bool isSame(BTAddr &bleAddr) {
      return !memcmp((uint8_t*)addr, (uint8_t*)bleAddr.addr, sizeof(esp_bd_addr_t));
    }

    void setAddr(BTAddr &bleAddr) {
      memcpy((uint8_t*)addr, bleAddr.addr, sizeof(esp_bd_addr_t));
    }
};


#endif