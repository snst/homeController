# Simple home automation solution to control an eQ-3 radiator thermostat over the internet



- Android application connects to a mqtt server and publishes the commands for the thermostat
- ESP32 NodeMCU connects via WLAN to the mqtt server and forwards the received commands to the thermostat via BLE.
