#include "MI2C.h"

MI2C::MI2C(TwoWire *i2c_)
{
    i2c = i2c_;
    //i2c -> begin(sda_,scl_,100000); //100khz
}

uint8_t MI2C::write(uint8_t addr_, uint8_t* buf_, uint8_t length_)
{
    i2c -> beginTransmission(addr_);
    for(int i = 0; i < length_; i++)
        i2c -> write(buf_[i]);
    return i2c -> endTransmission();
}
uint8_t MI2C::write(uint8_t addr_, uint8_t* buf_, uint8_t length_,bool sendStop)
{
    i2c -> beginTransmission(addr_);
    for(int i = 0; i < length_; i++)
        i2c -> write(buf_[i]);
    return i2c -> endTransmission(sendStop);
}

uint8_t MI2C::read(uint8_t addr_, uint8_t* buf_, uint8_t length_)
{
    int bytes = i2c -> requestFrom(addr_, length_);
    /*
    while (Wire.available())
    {
        for (int i = 0; i < length_; i++)
        {
            buf_[i] = Wire.read();
        }
    }  */ 
    uint8_t i = 0;
    while(i < bytes)
    {
        if(i2c -> available())
            buf_[i++] = i2c -> read();
    }
    return bytes;
}

uint8_t MI2C::alive(uint8_t addr_)
{
    i2c -> beginTransmission(addr_);
    if((i2c -> endTransmission()) == 0)
        return 1;
    else
        return 0;
}