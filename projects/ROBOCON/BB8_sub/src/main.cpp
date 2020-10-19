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
#include "ESP32mother.h"
#include "ESP32Servo.h"

#define MIN_PULSE 900
#define MAX_PULSE 2100
#define Step (MAX_PULSE - MIN_PULSE) / 120

Servo s[4];
Timer st[4], Dt;

volatile int angle[4];
volatile int target[4];
volatile int Speed[4];
volatile int8_t STEP[4];
volatile bool END[4];
const int servopin[4] = {enc1a, enc1b, enc2a, enc2b};

void Servobegin()
{
	for (int i = 0; i < 4; i++)
	{
		Speed[i] = 100;
		angle[i] = MIN_PULSE;
		target[i] = MIN_PULSE;
		STEP[i] = 1;
		END[i] = false;
		s[i].setPeriodHertz(50);
		s[i].attach(servopin[i]);
	}
}
void Servomove()
{
	for (int i = 0; i < 4; i++)
	{
		if (st[i].stand_by(1.f / (float)Speed[i]) && !END[i])
			angle[i] += STEP[i];
		if (angle[i] == target[i])
			END[i] = true;
		s[i].writeMicroseconds(angle[i]);
	}
}

void MAIN();
void Servotest();

void setup()
{
	odm.setup(TWOWHEEL);
	ESPtask_i2c_init = []() {
		mpu.setup();
		lider.setup();
	}; //i2cを使用するものの初期化
	ESPtask_i2c_while = []() {
		static int ct = 0;
		switch (ct)
		{
		case 0:
			lider.update();
			break;
		case 1:
			I2C_Error = (i2c_err_t)l1.com();
			mpu.update();
			ct = -1;
			break;
		}
		ct++;
	};		   //i2cを使用するものをsensor_interval[ms]ごとに実行
	ESPinit(); //ESP32mother Initialize
	/*task setup*/
	run.setfunction("ROBOCON MAIN task", MAIN);
	run.setfunction("Servo task", Servotest);
	Servobegin();
	Start_();
}

void loop()
{
	ESPUpdate();
}

void MAIN()
{
	if (StartFlag)
	{
		MELODY = true;
		SerialMonitor = true;
		StartFlag = false;
	}
	if (debug_t)
	{
		ESP_SERIAL.printf("Main\n\r");
	}
	if (EMARGENCYSTOP())
		return;
}
void Servotest()
{
	if (StartFlag)
	{
		MELODY = true;
		SerialMonitor = true;
		StartFlag = false;
	}
	if (debug_t)
	{
		ESP_SERIAL.printf("Main\n\r");
	}
	if (EMARGENCYSTOP())
		return;
	if (Dt.stand_by(0.5))
	{
		int nowdeg = 0;
		for (int i = 0; i < 4; i++)
		{
			Speed[0] = 100;
			Speed[1] = 10000;
			Speed[2] = 50;
			Speed[3] = 70;
			if (END[i])
			{
				nowdeg = (angle[i] - MIN_PULSE) / Step;
				if (i != 1)
					target[i] = (MIN_PULSE + (Step * 110) != target[i]) ? MIN_PULSE + (Step * 110) : MIN_PULSE;
				else
					target[i] = (MIN_PULSE + (Step * 30) != target[i]) ? MIN_PULSE + (Step * 30) : MIN_PULSE;
				STEP[i] = (target[i] - angle[i]) / abs(angle[i] - target[i]);
				END[i] = false;
			}
		}
	}
	Servomove();
}