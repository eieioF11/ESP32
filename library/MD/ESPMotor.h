#ifndef ESPMOTOR_H_
#define ESPMOTOR_H_
#include "TB6612FNG.h"
#include "MD.h"
#include "PID.h"
#include "Encoder.h"

#if defined __cplusplus
extern "C" {
#endif

#define PID_M 0
#define DUTY_M 1

class ESPMotor:public MD
{
    private:
        float speed;
        PID *pidmd;
        Encoder *e;
        TB6612FNG *t;
        uint8_t mode;
    public:
        float Kp,Ki,Kd;
        ESPMotor(int pin1_,int pin2_,gpio_num_t sp,int A,int B,int resolution_,pcnt_unit_t pcnt_,float dt);
        ESPMotor(int pin1_,int pin2_,gpio_num_t sp);
        void set();
        void Move(float val);
        void Update();
        void Reset();
        void Brake();
};

#if defined __cplusplus
}
#endif

#endif