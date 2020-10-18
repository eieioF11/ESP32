#include "ESP_peripheral.h"

MSPI spi(&SPI);
MI2C i2c(&Wire);
Odometry odm;
MPU9250 mpu;
Lider lider;
Lightsensor l1(&i2c,LSADDRA);
Chattering MOUNT(10,300,300,400);
Chattering STARTSWITCH(3,120);

MD *md[2];
MoveBase *wheel;

void motorsetup()
{
    #if (MOTORMODE ==PID_)
    md[0] = new ESPMotor(m1o1,m1o2,m1pwm,enc1a,enc1b,resolution,PCNT_UNIT_0,Delta_T);
    md[1] = new ESPMotor(m2o1,m2o2,m2pwm,enc2a,enc2b,resolution,PCNT_UNIT_1,Delta_T);
    #else
    md[0] = new ESPMotor(m1o1,m1o2,m1pwm);
    md[1] = new ESPMotor(m1o1,m1o2,m1pwm);
    #endif
    wheel = new Two_wheels(md);
    wheel->Stop();
}

/*Encoder*/
#if (MOTORMODE ==PID_)
void enctask(void *arg)
{
    portTickType lt = xTaskGetTickCount();
    while(1)
    {
        if(!OTAFLAG)
        {
            ((ESPMotor*)md[0])->Update();
            ((ESPMotor*)md[1])->Update();
        }
        vTaskDelayUntil(&lt,5/portTICK_RATE_MS);
    }
}
#endif

/*Odmetry*/
void Odmetryupdate(void *arg)
{
    portTickType lt = xTaskGetTickCount();
    while(1)
    {
        if(!OTAFLAG)
        {
            //odm.setsixaxis(mpu.read(AccX),mpu.read(AccY),mpu.read(AccZ),mpu.read(GyroX),mpu.read(GyroY),mpu.read(GyroZ));
            odm.setnineaxis(mpu.read(AccX),mpu.read(AccY),mpu.read(AccZ),mpu.read(GyroX),mpu.read(GyroY),mpu.read(GyroZ),mpu.read(MagX),mpu.read(MagY),mpu.read(MagZ));
            #if (MOTORMODE ==PID_)
            odm.setspeed(md[0]->now_val,md[1]->now_val);
            #endif
            odm.update();
        }
        vTaskDelayUntil(&lt,(sensor_interval*100)/portTICK_RATE_MS);
    }
}