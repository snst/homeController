/*
 * SoftI2CMaster.h -- Multi-instance software I2C Master library
 * 
 * 2010-2012 Tod E. Kurt, http://todbot.com/blog/
 * 2014, by Testato: update library and examples for follow Wire’s API of Arduino IDE 1.x
 *
 */

#ifndef SoftI2CMaster_h
#define SoftI2CMaster_h

#if 0

#include <inttypes.h>

#define _SOFTI2CMASTER_VERSION 13  // software version of this library


class SoftI2CMaster
{

public:
  // per object data
  uint8_t _sclPin;
  uint8_t _sdaPin;
  
  
  // public methods
  SoftI2CMaster();
  void setPins(uint8_t sclPin, uint8_t sdaPin, uint8_t usePullups);
  uint8_t requestFrom(uint8_t address, uint8_t quantity);
  uint8_t beginTransmission(uint8_t address);
  uint8_t write(uint8_t);
  uint8_t read();
  uint8_t endTransmission(void);

void I2C_delay(void);
bool read_SCL(void);  // Return current level of SCL line, 0 or 1
bool read_SDA(void);  // Return current level of SDA line, 0 or 1
void set_SCL(void);   // Do not drive SCL (set pin high-impedance)
void clear_SCL(void); // Actively drive SCL signal low
void set_SDA(void);   // Do not drive SDA (set pin high-impedance)
void clear_SDA(void); // Actively drive SDA signal low
void arbitration_lost(void);
void i2c_start_cond(void);
void i2c_stop_cond(void);
void i2c_write_bit(bool bit);
bool i2c_read_bit(void);
bool i2c_write_byte(bool send_start,
                    bool send_stop,
                    unsigned char byte);

unsigned char i2c_read_byte(bool nack, bool send_stop);

bool started;


};















#else

#include <inttypes.h>

#define _SOFTI2CMASTER_VERSION 13  // software version of this library


class SoftI2CMaster
{

public:
  // per object data
  uint8_t _sclPin;
  uint8_t _sdaPin;
  
  uint8_t usePullups;
  
  // 'initialized' will be:
  //    255 on startup,
  //    0 if beginTransmission() was called and successful,
  //    any other value if there was an error during beginTransmission().
  uint8_t initialized;

  // private methods

  void i2c_writebit( uint8_t c );
  uint8_t i2c_readbit(void);
  void i2c_init(void);
  void i2c_start(void);
  void i2c_repstart(void);
  void i2c_stop(void);
  uint8_t i2c_write( uint8_t c );
  uint8_t i2c_read( uint8_t ack );

  void i2c_scl_lo();
  void i2c_sda_lo();
  void i2c_scl_hi();
  void i2c_sda_hi();
  void i2c_scl_release();
  void i2c_sda_release();
  void _delay_us(uint32_t us);
  void scl_stretch();
  
public:
  // public methods
  SoftI2CMaster();
  SoftI2CMaster(uint8_t sclPin, uint8_t sdaPin);
  SoftI2CMaster(uint8_t sclPin, uint8_t sdaPin, uint8_t usePullups);

  void setPins(uint8_t sclPin, uint8_t sdaPin, uint8_t usePullups);

  uint8_t beginTransmission(uint8_t address);
  uint8_t beginTransmission(int address);
  uint8_t endTransmission(void);
  uint8_t write(uint8_t);
  void write(uint8_t*, uint8_t);
  void write(int);
  void write(char*);
  void begin(void) {return;};
  uint8_t requestFrom(int address);
  uint8_t requestFrom(uint8_t address);
  uint8_t requestFrom(int address, int quantity);
  uint8_t requestFrom(uint8_t address, uint8_t quantity);
  uint8_t read( uint8_t ack );
  uint8_t read();
  uint8_t readLast();

};
#endif
#endif