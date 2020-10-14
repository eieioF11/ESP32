#include "MI2C.h"

const char* i2cerror(i2c_err_t err)
{
    switch(err)
    {
        case I2C_ERROR_OK:return"I2C_ERROR_OK";
        case I2C_ERROR_DEV:return"I2C_ERROR_DEV";
        case I2C_ERROR_ACK:return"I2C_ERROR_ACK";
        case I2C_ERROR_TIMEOUT:return"I2C_ERROR_TIMEOUT";
        case I2C_ERROR_BUS:return"I2C_ERROR_BUS";
        case I2C_ERROR_BUSY:return"I2C_ERROR_BUSY";
        case I2C_ERROR_MEMORY:return"I2C_ERROR_MEMORY";//overflow
        case I2C_ERROR_CONTINUE:return"2C_ERROR_CONTINUE";
        case I2C_ERROR_NO_BEGIN:return"I2C_ERROR_NO_BEGIN";
    }
    return "I2COK";
}

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