#include <Lightsensor.h>

Lightsensor::Lightsensor(MI2C *i2c_, uint8_t addr_)
{
    i2c = i2c_;
    addr = addr_;
    mode = 0b00111111;
}

int Lightsensor::com()
{
    //printf("%d,%d,%d,%d,%d,%d\n",rdata[0],rdata[1],rdata[2],rdata[3],rdata[4],rdata[5]);
    i2c -> write(addr,&mode,1);
    return i2c -> read(addr,rdata,6);
}

uint8_t Lightsensor::read(int num_)
{
    switch(num_)
    {
        case 0:
        case 1:
        case 2:
        case 3:
        case 4:
        case 5:
            return rdata[num_];
            break;
    }
    return 0;
}
bool Lightsensor::swread(uint8_t select)
{
    if(rdata[5]&1<<select)return 0;
    else return 1;
}