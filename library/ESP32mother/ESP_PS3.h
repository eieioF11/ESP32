/*
geinsetup mode
kybode comand
'm':motor select
'p':Pgain set mode      
'i':Igain set mode
'd':dgain set mode
'r':md1 gain set
'l':md2 gain set
' ':gain save
'q':mode reset
*/
#ifndef ESP_PS3_H_
#define ESP_PS3_H_
#include "Setup.h"
#include <Arduino.h>
#include "math.h"
#include "MPU9250.h"
#include "Timer.h"
#include "stprint.h"

#if defined __cplusplus
extern "C" {
#endif

#if (ESP_MODE == ESP_PS3)
#include "Ps3Controller.h"
#define MAC_ADD (char*)"24:0a:c4:0f:c3:aa"

typedef enum
{
  lX,
  lY,
  rX,
  rY,
}ps3stickmode;

extern String PS3Debug;
void PS3init();  //setup()で１回実行
void PS3Update();
bool PS3Controller(float *Vx,float *Vy,float *Angular);//-1~1
uint16_t PS3Button();//0b/select/start/l3/r3/l1/r1/l2/r2/up/down/right/left/△/〇/×/□/
bool PS3PSButton();
int8_t PS3stick(ps3stickmode mode);
bool PS3Conect();
#endif



#if defined __cplusplus
}
#endif
#endif