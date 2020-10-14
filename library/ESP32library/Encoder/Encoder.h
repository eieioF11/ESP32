//Ver 1.0
#ifndef _ENCODER_H_
#define _ENCODER_H_

#include "Arduino.h"
#include <stdio.h>
#include <stdlib.h>
#include "esp_system.h"
#include "esp_timer.h"
#include "driver/pcnt.h"

#define rad false
#define deg true
#define PCNT_H_LIM_VAL 10000 //カウンタの上限
#define PCNT_L_LIM_VAL -10000 //カウンタの下限

class Encoder
{
    private:
        int pinA,pinB,p;
        unsigned long  nowtime,oldtime;
        int Resolution;
        int16_t sum;
        float rps;
        int8_t rev;
        pcnt_unit_t pcnt;
    public:
        Encoder(int A,int B,int resolution_,pcnt_unit_t pcnt_);
        void Reverse(bool);
        void reset();
        void spreset();//速度のみリセット
        int16_t get_sum();//注意　Update();の実行間隔で初期化される
        float get_rotation();
        float get_angle(bool mode);
        float get_rps();//速度[rps]を返す
        float get_w();//各速度[rad/s]を返す
        void Update();//一定間隔で実行すること
};
/*
Ex

#define pinA   21
#define pinB   22
#define resolution 2048.0f

Encoder e(pinA,pinB,resolution,PCNT_UNIT_0);

void enctask(void *arg)
{
    portTickType lt = xTaskGetTickCount();
    while(1)
    {
        e.Update();
        vTaskDelayUntil(&lt,5/portTICK_RATE_MS);
    }
}

void setup()
{
    xTaskCreatePinnedToCore(enctask,"enctask",4048,NULL,2,NULL,1);
}

void loop()
{
    printf("%f[rps]\n",e.get_rps());
}
*/
#endif