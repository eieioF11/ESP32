#ifndef ESP_SENSOR_H_
#define ESP_SENSOR_H_
/*Standardm*/
#include <Arduino.h>
#include <stdio.h>
#include "math.h"
#include "Setup.h"
#include "Mbed.h"
/*Passive*/
#include "Lider.h"
#include "Lightsensor.h"
#include "SparkFunMPU9250-DMP.h"
#include "Encoder.h"
/*Active*/
#include "Two_wheels.h"
#include "MD.h"
#include "MoveBase.h"
#include "ESPMotor.h"
#include "Sound.h"
/*Calculation, correction*/
#include "odometry.h"
#include "lowpassfilter.h"
#include "Chattering.h"

//電源パラメーター
#define PowerV_Max 10.5f //Battery voltage(NIMH x 8 = 10.5[v])
#define PowerV_Min 7.0f
#define LowLevel   60.0f

#if defined __cplusplus
    extern "C" {
#endif
//Serial
extern MSPI spi;
extern MI2C i2c;
//Sensor
extern Lightsensor l1;
extern MPU9250_DMP imu;
extern Lider lider;
//Odometry
extern Odometry odm;
//Button
extern Chattering MOUNT;
extern Chattering STARTSWITCH;
//Undercarriage
extern MD *md[2];
extern MoveBase *wheel;
//
extern bool sensor_ini;
//Task
void enctask(void *arg);
void Odmetryupdate(void *arg);
void motorsetup();

//Power Management
inline void Vmonitor_setup()
{
  pinMode(MOTOR_V, ANALOG);
}

inline float Vmonitor()
{
  int em=1;
  const float VM_RANGE=0.76f;
  //const float R1=2.2*1000;
  //const float R2=1.0*1000;
  const float R1=100*1000;
  const float R2=24*1000;
  float ADCinV=(((float)analogRead(MOTOR_V)/4095.f)*3.3);
  if(ADCinV<=0.f)
    em=0;
  float MPV=ADCinV*((R1+R2)/R2)+(VM_RANGE*em);
  return MPV;
}

inline float BatteryLevel()
{
  const float n=10;
  static float sum=0.0f;
  static float ave=0.0f;
  static int i=0;
  float level=0.f;
  float BTV=Vmonitor();
  if(BTV>PowerV_Max)
    BTV=PowerV_Max;
  if(BTV<PowerV_Min)
    BTV=PowerV_Min;
  level=mapfloat(BTV,PowerV_Min,PowerV_Max,0,100);
  i++;
  sum+=level;
  if(level!=0&&ave==0)
    ave=level;
  if(i>=n)
  {
    ave=sum/n;
    sum=0.f;
    i=0;
  }
  return ave;
}

inline bool LowV()
{
  if(Vmonitor()<=4.7f&&Vmonitor()>=3.0)
    return true;
  return false;
}

inline bool Emergency_Stop()
{
  if(Vmonitor()<=3.0f)
    return true;
  else
    return false;
}


inline float CPUTemp()
{
  return (float)temperatureRead();
}

#if defined __cplusplus
}
#endif
#endif