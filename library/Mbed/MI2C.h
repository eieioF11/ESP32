#ifndef MI2C_H_
#define MI2C_H_
#include <Arduino.h>
#include <Wire.h>

#if defined __cplusplus
extern "C" {
#endif

const char* i2cerror(i2c_err_t err);

class MI2C{
    public:
    TwoWire *i2c;
    MI2C(TwoWire *i2c_);
    uint8_t write(uint8_t addr_, uint8_t* buf_, uint8_t length_);
    uint8_t write(uint8_t addr_, uint8_t* buf_, uint8_t length_,bool sendStop);
    uint8_t read(uint8_t addr_, uint8_t* buf_, uint8_t length_);
    uint8_t alive(uint8_t addr_);
};

#if defined __cplusplus
}
#endif

#endif