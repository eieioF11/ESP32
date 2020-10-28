#ifndef SERVOSPEED_H_
#define SERVOSPEED_H_
/*Standardm*/
#include <Arduino.h>
#include <stdio.h>
#include "math.h"
#include "ESP32Servo.h"
#include "Timer.h"
#include "Setup.h"

#define MIN_PULSE 900
#define MAX_PULSE 2100
#define MAX_ANGLE 120

#if defined __cplusplus
extern "C" {
#endif

class ServoSpeed
{
	private:
		Servo s;
		Timer st;
		int speed;
		int target;
		int angle;
		float nowdeg;
		int step;
		bool flag;
		int max,min;
		bool stop;
		bool start;
	public:
		ServoSpeed();
		void begin(int pin,int min,int max);
		bool set(int angle,int speed);
		void setMs(int val);
		float nowangle();
        int nowMs();
		void update();
};

#if defined __cplusplus
}
#endif
#endif