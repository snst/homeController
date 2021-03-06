#include <Arduino.h>
#include <Wire.h>

class IDeviceI2C;

namespace Commands
{
    const uint8_t Measure_Temperature = 0xE3;
    const uint8_t Measure_Humidity = 0xE5;
    const uint8_t Measure_Temperature_NoHold = 0xF3;
    const uint8_t Measure_Humidity_NoHold  = 0xF5;
    const uint8_t Write_User_Register = 0xE6;
    const uint8_t Read_User_Register = 0xE7;
    const uint8_t Reset = 0xFE;
};


enum Resolution : uint8_t
{
    Rh12_Temp14 = 0x00,
    Rh8_Temp12 = 0x01,
    Rh10_Temp13 = 0x80,
    Rh11_Temp11 = 0x81
};

const uint8_t HTU21D_Default_Address = 0x40;  //Unshifted 7-bit I2C address for the sensor
const uint8_t Resolution_Mask = 0x81;
const uint8_t UserRegister_Default = 0x3A; //0x02;


class HTU21D {
    
    public:
        ~HTU21D();
        HTU21D();
        bool begin(IDeviceI2C *_i2c);
        bool reset(); 
        /*
        float getHumidity();
        float getTemperature();
        float getDewPoint();
        float getHeatIndex();
        */
        float readHumidity();
        float readTemperature();

    private:
        uint8_t readOutput(uint8_t *arrPtr, uint8_t Command, uint8_t len);
        uint8_t readUserRegister();
        void writeUserRegister(uint8_t RegData);
        bool ValidCyclicRedundancyCheck(uint16_t data, uint8_t crc);
        void setResolution(Resolution res);
        Resolution resolution;
        int _sensorAddress;
        IDeviceI2C *wire;
};