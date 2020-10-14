#include <Arduino.h>
#include <Wire.h>
#include "MD.h"
#include "Mbed.h"

#define T 0.2
#define g 9.8

#define MPU6050_ADDR 0x68
#define MPU6050_AX  0x3B
#define MPU6050_AY  0x3D
#define MPU6050_AZ  0x3F
#define MPU6050_TP  0x41    //  data not used
#define MPU6050_GX  0x43
#define MPU6050_GY  0x45
#define MPU6050_GZ  0x47

short int AccX, AccY, AccZ;
short int Temp;
short int GyroX, GyroY, GyroZ;
short rad,data[100],sum,ave,i,gy;
double Grad,Arad;
void GY_521set()
{
    Wire.beginTransmission(MPU6050_ADDR);
    Wire.write(0x6B);
    Wire.write(0);
    Wire.endTransmission();
}
void GY_521()
{
    //  send start address
    Wire.beginTransmission(MPU6050_ADDR);
    Wire.write(MPU6050_AX);
    Wire.endTransmission();  
    //  request 14bytes (int16 x 7)
    Wire.requestFrom(MPU6050_ADDR, 14);
    //  get 14bytes
    AccX = Wire.read() << 8;  AccX |= Wire.read();
    AccY = Wire.read() << 8;  AccY |= Wire.read();
    AccZ = Wire.read() << 8;  AccZ |= Wire.read();
    Temp = Wire.read() << 8;  Temp |= Wire.read();  //  (Temp-12421)/340.0 [degC]
    GyroX = Wire.read() << 8; GyroX |= Wire.read();
    GyroY = Wire.read() << 8; GyroY |= Wire.read();
    GyroZ = Wire.read() << 8; GyroZ |= Wire.read();
    for(i=0;i<100;i++){data[i]=GyroY/1000;}
    for(i=0;i<100;i++){sum+=data[i];}
    ave=sum/100;
    Grad += ave*T;
    Arad = asin((-0.001*AccY)/137);
}
void debug()
{
    Serial.print("AccX:"); Serial.print(AccX);
    Serial.print(",AccY:"); Serial.print(AccY);
    Serial.print(",AccZ:"); Serial.print(AccZ);
    Serial.print(",GyroX:"); Serial.print(GyroX);
    Serial.print(",GyroY:"); Serial.print(GyroY);
    Serial.print(",GyroZ:"); Serial.print(GyroZ);
    Serial.print(",ave:"); Serial.print(ave);
    Serial.print(",rad:"); Serial.print(rad);
    Serial.print(",Arad:"); Serial.print(Arad);
    Serial.print(",Grad:"); Serial.print(Grad);  
    Serial.println("");
}
SPI spi(GPIO_NUM_32, GPIO_NUM_33, GPIO_NUM_25);
MD m1(&spi,GPIO_NUM_16);
MD m2(&spi,GPIO_NUM_4);
void setup() {
    //  serial for debugging
    Serial.begin(115200);
    //  i2c as a master
    Wire.begin(27,26);
    GY_521set();
}
void loop() {
    GY_521();
    debug();
    //Serial.write(rad);
    if(rad>11000)
    {
        m1=0;
        m2=0;
        //Serial.write(0);
        delay(5000);
        rad=0;
    }
    else
    {
        m1=0.1;
        m2=0.1;
    }
    delay(T*1000);
}