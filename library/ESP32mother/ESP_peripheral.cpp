#include "ESP_peripheral.h"

MSPI spi(&SPI);
MI2C i2c(&Wire);
Odometry odm;
MPU9250_DMP imu;
Lider lider;
Lightsensor l1(&i2c,LSADDRA);
Chattering MOUNT(10,300,300,400);
Chattering STARTSWITCH(3,120);

MD *md[2];
MoveBase *wheel;

bool sensor_ini=true;

void motorsetup()
{
    #if (MOTORMODE ==PID_M)
    md[0] = new ESPMotor(m1o1,m1o2,m1pwm,enc1a,enc1b,resolution,PCNT_UNIT_0,Delta_T);
    md[1] = new ESPMotor(m2o1,m2o2,m2pwm,enc2a,enc2b,resolution,PCNT_UNIT_1,Delta_T);
    #else
    md[0] = new ESPMotor(m1o1,m1o2,m1pwm);
    md[1] = new ESPMotor(m2o1,m2o2,m2pwm);
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
    printf("sensor init");
    while(sensor_ini)
    {
        printf(".");
        delay(50);
    }
    odm.setup(TWOWHEEL);
    portTickType lt = xTaskGetTickCount();
    while(1)
    {
        if(!OTAFLAG)
        {
            odm.setposture(-1*imu.pitch,-1*imu.roll,-1*imu.yaw);
            #if (MOTORMODE ==PID_)
                odm.setspeed(md[0]->now_val, md[1]->now_val);
            #endif
            odm.update();
        }
        vTaskDelayUntil(&lt,(sensor_interval*100)/portTICK_RATE_MS);
    }
}