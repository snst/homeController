// Copyright 2017 Stefan Schmidt

#ifndef _HOMECONFIG_H__
#define _HOMECONFIG_H__

#include <Preferences.h>

#define PREF_APPNAME "myHome"
#define PREF_WLAN_SSID "WLAN_SSID"
#define PREF_WLAN_PW "WLAN_PW"
#define PREF_MQTT_SERVER "mqtt_server"
#define PREF_MQTT_PORT "mqtt_port"
#define PREF_MQTT_USER "mqtt_user"
#define PREF_MQTT_PW "mqtt_pw"
#define PREF_MQTT_TOPIC "mqtt_topic"

class HomeConfig {

  protected:
    Preferences preferences;

  public:
    String wlan_ssid;
    String wlan_pw;
    String mqtt_server;
    String mqtt_port;
    String mqtt_user;
    String mqtt_pw;
    String mqtt_topic;
    String mqtt_topic_status;
    String mqtt_topic_request;
  
  
  boolean getInput(char* str, const char* old, char* input) {
    int i=0;
    Serial.print(str);
    if(old!=nullptr) {
      Serial.print(" (");
      Serial.print(old);
      Serial.print(")");
    }
    Serial.print(" : ");
    while(true) {
  
      while(Serial.available()) {
        char ch = Serial.read();
        if (ch == '\n') {
          input[i] = 0;
          Serial.println("");
          return i>0;
        }
        else {
          input[i++] = ch;
          Serial.print(ch);
        }
      }
    }
  }

  
  bool updatePerfData(char* key, const char* old) {
    char buf[30];  
    bool ok = getInput(key, old, buf);
    if(ok) {
      preferences.putString(key, buf);
    }
    return ok;  
  }

  
  void loadPrefData(char* key, String& data) {
    data = preferences.getString(key);
    Serial.print(key);
    Serial.print(": ");
    Serial.println(data.c_str());
  }
  
  
  void doConfiguration() {
  
    preferences.begin(PREF_APPNAME, false);
    Serial.println("** INPUT CONFIG **");
    updatePerfData(PREF_WLAN_SSID, wlan_ssid.c_str());
    updatePerfData(PREF_WLAN_PW, wlan_pw.c_str());
    updatePerfData(PREF_MQTT_SERVER, mqtt_server.c_str());
    updatePerfData(PREF_MQTT_PORT, mqtt_port.c_str());
    updatePerfData(PREF_MQTT_USER, mqtt_user.c_str());
    updatePerfData(PREF_MQTT_PW, mqtt_pw.c_str());
    updatePerfData(PREF_MQTT_TOPIC, mqtt_topic.c_str());
    Serial.println("Saving..");
    preferences.end();
    Serial.println("Do reset!");
    while(true) {};
  }
  
  
  void loadSettings() {
    Serial.println("** LOAD CONFIG **");
    preferences.begin(PREF_APPNAME, true);
    loadPrefData(PREF_WLAN_SSID, wlan_ssid);
    loadPrefData(PREF_WLAN_PW, wlan_pw);
    loadPrefData(PREF_MQTT_SERVER, mqtt_server);
    loadPrefData(PREF_MQTT_PORT, mqtt_port);
    loadPrefData(PREF_MQTT_USER, mqtt_user);
    loadPrefData(PREF_MQTT_PW, mqtt_pw);
    loadPrefData(PREF_MQTT_TOPIC, mqtt_topic);
    preferences.end();

    mqtt_topic_status = mqtt_topic + "/" + "stat";
    mqtt_topic_request = mqtt_topic + "/" + "req";
    Serial.println("");
  }


  void userInput(int ms) {
    Serial.println("Press return for config\n");
  
    delay(ms);
    while(Serial.available() > 0) {
      if (Serial.read() == '\n') {
        doConfiguration();
      }
    }
  }
};

#endif
