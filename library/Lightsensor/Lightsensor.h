#ifndef LIGHTSENSOR_H_
#define LIGHTSENSOR_H_
#include "Mbed.h"

#define LSADDRF 0x0f
#define LSADDRB 0x0b
#define LSADDRA 0x0a

#define sw1 0
#define sw2 1

#if defined __cplusplus
extern "C" {
#endif

class Lightsensor{
    private:
    MI2C *i2c;
    uint8_t rdata[6]={0};
    uint8_t addr;
    uint8_t mode;
    public:
    Lightsensor(MI2C *i2c_, uint8_t addr_);
    int com();
    uint8_t read(int num_);
    bool swread(uint8_t select);
};

#if defined __cplusplus
}
#endif

#endif