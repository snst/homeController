#ifndef _BME280_H_
#define _BME280_H_

class MqttHandler;

class Bme280 {

public:
    Bme280(MqttHandler &m);
    void init();
    bool execute();
    MqttHandler & mqttHandler;
};

#endif