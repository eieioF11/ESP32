#ifndef __URURUMD_H__
#define __URURUMD_H__
#include "MD.h"
#include <Arduino.h>
#include <SPI.h>
#include <math.h>
#define SPEED_MODE      0   //default
#define ANGLE_MODE      1   
#define DUTY_MODE       2

const int DataNum = 4;
#define Polynomial      0xd5//CRC8
#define sampleT         50.00E-3//100.00E-3  //[s]
#define PulseResolution 512
#define AngleResolution 4096

//温度検知回路のパラメータ　使用サーミスタ->NCP18XQ102
#define Rth0    1.00E3  //基準抵抗[Ω]
#define T0      293.15  //基準温度[K]
#define Bconst  3706.0  //B定数(25-100℃)[K]
#define Rp      4.7E3   //プルアップ抵抗[Ω]
#define FVR     1.024   //ADC基準電圧[V]
#define Vin     3.3     //MDのマイコン電源電圧[V]

//電圧検知閾値
#define V_Threshold 800

typedef enum{
  DoNothing = 0,
  Information,
  Reset,
  Brake, 
  SetDuty,

  PID_SetSpeed,
  PID_SetAngle,

  PID_SetGainK1,
  PID_SetGainK2,
  PID_SetGainK3,

  PID_AutoTuneGain,
  PID_SaveSetting,
}MD_Command_t;

typedef enum{
  Nothing = 0,
  RotaryEncorder,
  PulseSum,
  H_Bridge_Tempreture,  
  H_Bridge_Voltage,
  PID_GainK1,
  PID_GainK2,
  PID_GainK3,
}MD_Inquire_t;

typedef union{
  uint8_t byte[4];
  struct{
    uint8_t   dummy;
    uint16_t  val;
    uint8_t   crc;
  }__packed data;
}MD_Send_t;

typedef union{
  uint8_t byte[4];
  struct{
    uint8_t   cmd;
    uint16_t  val;
    uint8_t   crc;
  }__packed data;
}MD_Receive_t;

class ururuMD : public MD{
    public:
    ururuMD();
    ururuMD(uint8_t cs);
    ururuMD(uint8_t cs, int ControlMode);

    void Move(double val);
    
    //Set PID_Gain
    void SetGain();
    void GetGain();
    double GetTempureture();
    bool VoltageDetect();

    uint8_t Send(MD_Command_t cmd, MD_Inquire_t inq, uint16_t val);

    //PID Gain 
    double Kp, Ki, Kd;
    int16_t K[3];
    private:
    uint8_t CRC8(uint8_t *data, int size); 
    //PID Gain (MathAcc)
    int Mode;
    uint8_t CS; 
    MD_Send_t data;
};

#endif