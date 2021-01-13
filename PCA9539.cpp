
#if defined(ARDUINO) && ARDUINO >= 100
#include "Arduino.h"
#else
#include "WProgram.h"
#endif

#include "PCA9539.h"
#include "Wire.h"

PCA9539::PCA9539(uint8_t address) {
    _address         = address;        // save the address id
    _valueRegister   = 0;
    //Wire.begin(D1,D2);                      // start I2C communication
}


void PCA9539::pinMode(uint8_t pin, uint8_t IOMode) {

    //
    // check if valid pin first
    //
    if (pin <= 15) {
        //
        // now set the correct bit in the configuration register
        //
        if (IOMode == OUTPUT) {
            //
            // mask correct bit to 0 by inverting x so that only
            // the correct bit is LOW. The rest stays HIGH
            //
            _configurationRegister = _configurationRegister & ~(1 << pin);
        } else {
            //
            // or just the required bit to 1
            //
            _configurationRegister = _configurationRegister | (1 << pin);
        }
        //
        // write configuration register to chip
        //
        I2CSetValue(_address, NXP_CONFIG    , _configurationRegister_low);
        I2CSetValue(_address, NXP_CONFIG + 1, _configurationRegister_high);
    }
}

uint8_t PCA9539::digitalRead(uint8_t pin) {
    uint16_t _inputData = 0;
    //
    // we wil only process pins <= 15
    //
    if (pin > 15 ) return 255;
    _inputData  = I2CGetValue(_address, NXP_INPUT);
    _inputData |= I2CGetValue(_address, NXP_INPUT + 1) << 8;
    //
    // now mask the bit required and see if it is a HIGH
    //
    if ((_inputData & (1 << pin)) > 0){
        //
        // the bit is HIGH otherwise we would return a LOW value
        //
        return HIGH;
    } else {
        return LOW;
    }
}

void PCA9539::digitalWrite(uint8_t pin, uint8_t value) {
    //
    // check valid pin first
    //
    if (pin > 15 ){
        _error = 255;            // invalid pin
        return;                  // exit
    }
    //
    // if the value is LOW we will and the register value with correct bit set to zero
    // if the value is HIGH we will or the register value with correct bit set to HIGH
    //
    if (value > 0) {
        //
        // this is a High value so we will or it with the value register
        //
        _valueRegister = _valueRegister | (1 << pin);    // and OR bit in register
    } else {
        //
        // this is a LOW value so we have to AND it with 0 into the _valueRegister
        //
        _valueRegister = _valueRegister & ~(1 << pin);    // AND all bits
    }
    I2CSetValue(_address, NXP_OUTPUT    , _valueRegister_low);
    I2CSetValue(_address, NXP_OUTPUT + 1, _valueRegister_high);
}
//
// low level hardware methods
//
uint16_t PCA9539::I2CGetValue(uint8_t address, uint8_t reg) {
    uint16_t _inputData;
    //
    // read the address input register
    //
    Wire.beginTransmission(address);          // setup read registers
    Wire.write(reg);
    _error = Wire.endTransmission();
    //
    // ask for 2 bytes to be returned
    //
    if (Wire.requestFrom((int)address, 1) != 1)
    {
        //
        // we are not receing the bytes we need
        //
        return 256;                            // error code is above normal data range
    };
    //
    // read both bytes
    //
    _inputData = Wire.read();
    return _inputData;
}


void PCA9539::I2CSetValue(uint8_t address, uint8_t reg, uint8_t value){
    //
    // write output register to chip
    //
    Wire.beginTransmission(address);              // setup direction registers
    Wire.write(reg);                              // pointer to configuration register address 0
    Wire.write(value);                            // write config register low byte
    _error = Wire.endTransmission();
}

