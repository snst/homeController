/*
 * SoftI2CMaster.cpp -- Multi-instance software I2C Master library
 * 
 * 
 * 2010-12 Tod E. Kurt, http://todbot.com/blog/
 *
 * This code takes some tricks from:
 *  http://codinglab.blogspot.com/2008/10/i2c-on-avr-using-bit-banging.html
 *
 * 2014, by Testato: update library and examples for follow Wire’s API of Arduino IDE 1.x
 *
 */

#include <Arduino.h>
#include "SoftI2CMaster.h"
#include <string.h>

#define  i2cbitdelay 50

#define  I2C_ACK  1 
#define  I2C_NAK  0


//
// Constructor
//
SoftI2CMaster::SoftI2CMaster()
{
    // do nothing, use setPins() later
}
//
SoftI2CMaster::SoftI2CMaster(uint8_t sclPin, uint8_t sdaPin) 
{
    setPins(sclPin, sdaPin, true);
}

//
SoftI2CMaster::SoftI2CMaster(uint8_t sclPin, uint8_t sdaPin, uint8_t pullups)
{
    setPins(sclPin, sdaPin, pullups);
}

//
// Turn Arduino pin numbers into PORTx, DDRx, and PINx
//
void SoftI2CMaster::setPins(uint8_t sclPin, uint8_t sdaPin, uint8_t pullups)
{
    uint8_t port;
    
    usePullups = pullups;

    _sclPin = sclPin;
    _sdaPin = sdaPin;

    pinMode(_sclPin, OUTPUT);    
    pinMode(_sdaPin, OUTPUT);    
       
    initialized = 255;
    i2c_init();
}

void SoftI2CMaster::i2c_scl_release()
{

}

void SoftI2CMaster::i2c_sda_release()
{

}

void SoftI2CMaster::i2c_scl_lo()
{
    digitalWrite(_sclPin, LOW);
}

void SoftI2CMaster::i2c_sda_lo()
{
    digitalWrite(_sdaPin, LOW);
}

void SoftI2CMaster::i2c_scl_hi()
{
    digitalWrite(_sclPin, HIGH);
}

void SoftI2CMaster::i2c_sda_hi()
{
    digitalWrite(_sdaPin, HIGH);
}

void SoftI2CMaster::_delay_us(uint32_t us)
{
    delayMicroseconds(us);
}

//
//
//
uint8_t SoftI2CMaster::beginTransmission(uint8_t address)
{
    i2c_start();
    uint8_t rc = i2c_write((address<<1) | 0); // clr read bit
    // The standard Wire library returns a status in endTransmission(), not beginTransmission().
    // So we will return the status here but also remember the result so we can return it in endTransmission().
    // It also allows us to disable other I2C functions until beginTransmission has been called, if we want.
    initialized = rc;
    return rc;
}

//
uint8_t SoftI2CMaster::requestFrom(uint8_t address)
{
    i2c_start();
    uint8_t rc = i2c_write((address<<1) | 1); // set read bit
    return rc;
}
//
uint8_t SoftI2CMaster::requestFrom(int address)
{
    return requestFrom( (uint8_t) address);
}
// Added for compatibility with the standard Wire library.
uint8_t SoftI2CMaster::requestFrom(int address, int quantity)
{
    return requestFrom( (uint8_t) address);

    // Ignore 'quantity', since SoftI2CMaster::requestFrom() just sets the start of read adresses,
    // so it's the same for any number of bytes.
    (void)quantity;
}
// Added for compatibility with the standard Wire library.
uint8_t SoftI2CMaster::requestFrom(uint8_t address, uint8_t quantity)
{
    return requestFrom( (uint8_t) address);

    // Ignore 'quantity', since SoftI2CMaster::requestFrom() just sets the start of read adresses,
    // so it's the same for any number of bytes.
    (void)quantity;
}

//
uint8_t SoftI2CMaster::beginTransmission(int address)
{
    return beginTransmission((uint8_t)address);
}

//
//
//
uint8_t SoftI2CMaster::endTransmission(void)
{
    i2c_stop();
    return initialized;   // Use the result of beginTransmission()
}

// must be called in:
// slave tx event callback
// or after beginTransmission(address)
uint8_t SoftI2CMaster::write(uint8_t data)
{
    return i2c_write(data);
}

// must be called in:
// slave tx event callback
// or after beginTransmission(address)
void SoftI2CMaster::write(uint8_t* data, uint8_t quantity)
{
    for(uint8_t i = 0; i < quantity; ++i){
        write(data[i]);
    }
}

// must be called in:
// slave tx event callback
// or after beginTransmission(address)
void SoftI2CMaster::write(char* data)
{
    write((uint8_t*)data, strlen(data));
}

// must be called in:
// slave tx event callback
// or after beginTransmission(address)
void SoftI2CMaster::write(int data)
{
    write((uint8_t)data);
}

//--------------------------------------------------------------------


void SoftI2CMaster::i2c_writebit( uint8_t c )
{
    if ( c > 0 ) {
        i2c_sda_hi();
    } else {
        i2c_sda_lo();
    }

    _delay_us(i2cbitdelay);
  
    i2c_scl_hi();
  
    _delay_us(i2cbitdelay);

    i2c_scl_lo();
//    _delay_us(i2cbitdelay);

    /*if ( c > 0 ) {
        i2c_sda_lo();
    }*/
//    _delay_us(i2cbitdelay);
}

void SoftI2CMaster::scl_stretch()
{
    int n=500;
    do {
        _delay_us(5);
    } while( !digitalRead(_sclPin) && n--);
}


//
uint8_t SoftI2CMaster::i2c_readbit(void)
{
    i2c_sda_hi();

    _delay_us(i2cbitdelay);

    i2c_scl_hi();

    scl_stretch();

    _delay_us(i2cbitdelay);

    bool bit = digitalRead(_sdaPin);

    i2c_scl_lo();
//    _delay_us(i2cbitdelay);

    return bit ? 1 : 0;
}

// Inits bitbanging port, must be called before using the functions below
//
void SoftI2CMaster::i2c_init(void)
{
    i2c_sda_hi();
    i2c_scl_hi();
    
    _delay_us(i2cbitdelay);
}

// Send a START Condition
//
void SoftI2CMaster::i2c_start(void)
{
    // set both to high at the same time
    i2c_sda_hi();
    i2c_scl_hi();
    _delay_us(i2cbitdelay/2);
   
    i2c_sda_lo();
    _delay_us(i2cbitdelay/2);

    i2c_scl_lo();
  //  _delay_us(i2cbitdelay);
}

void SoftI2CMaster::i2c_repstart(void)
{
    // set both to high at the same time (releases drive on both lines)
    i2c_sda_hi();
    i2c_scl_hi();

    i2c_scl_lo();                           // force SCL low
    _delay_us(i2cbitdelay);

    i2c_sda_release();                      // release SDA
    _delay_us(i2cbitdelay);

    i2c_scl_release();                      // release SCL
    _delay_us(i2cbitdelay);

    i2c_sda_lo();                           // force SDA low
    _delay_us(i2cbitdelay);
}

// Send a STOP Condition
//
void SoftI2CMaster::i2c_stop(void)
{
    i2c_scl_hi();
    _delay_us(i2cbitdelay/2);

    i2c_sda_hi();
    _delay_us(i2cbitdelay/2);
}

// write a byte to the I2C slave device
//
uint8_t SoftI2CMaster::i2c_write( uint8_t c )
{
    for ( uint8_t i=0;i<8;i++) {
        i2c_writebit( c & 128 );
        c<<=1;
    }
   // i2c_sda_hi(); // stsc??

    return i2c_readbit();
}

// read a byte from the I2C slave device
//
uint8_t SoftI2CMaster::i2c_read( uint8_t ack )
{
    uint8_t res = 0;

    for ( uint8_t i=0;i<8;i++) {
        res <<= 1;
        res |= i2c_readbit();  
    }

    if ( ack )
        i2c_writebit( 0 );
    else
        i2c_writebit( 1 );

    _delay_us(i2cbitdelay);

    return res;
}

// FIXME: this isn't right, surely
uint8_t SoftI2CMaster::read( uint8_t ack )
{
  return i2c_read( ack );
}

//
uint8_t SoftI2CMaster::read()
{
    return i2c_read( I2C_ACK );
}

//
uint8_t SoftI2CMaster::readLast()
{
    return i2c_read( I2C_NAK );
}

