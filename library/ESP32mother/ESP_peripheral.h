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
#include "MPU9250.h"
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

#if defined __cplusplus
extern "C" {
#endif
//Serial
extern MSPI spi;
extern MI2C i2c;
//Sensor
extern Lightsensor l1;
extern MPU9250 mpu;
extern Lider lider;
//Odometry
extern Odometry odm;
//Button
extern Chattering MOUNT;
extern Chattering STARTSWITCH;
//Undercarriage
extern MD *md[2];
extern MoveBase *wheel;

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
  const float R1=2.2*1000;
  const float R2=1.0*1000;
  float ADCinV=(((float)analogRead(MOTOR_V)/4095.f)*3.3);
  if(ADCinV<=0.f)
    em=0;
  float MPV=ADCinV*((R1+R2)/R2)+(VM_RANGE*em);
  return MPV;
}

inline bool Emergency_Stop()
{
  if(Vmonitor()<=0.f)
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