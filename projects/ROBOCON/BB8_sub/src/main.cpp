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

class ServoSpeed
{
	private:
		Servo s;
		Timer st;
		int speed;
		int target;
		int angle;
		int nowdeg;
		int step;
		bool flag;
		int max,min;
	public:
		ServoSpeed();
		void begin(int pin,int min,int max);
		bool set(int angle,int speed);
		int nowangle();
		void update();
};
ServoSpeed::ServoSpeed(){}
void ServoSpeed::begin(int pin,int min,int max)
{
	speed = 100;
	this->min=min;
	this->max=max;
	angle = min;
	target = min;
	step = 1;
	flag = true;
	s.setPeriodHertz(50);
	s.attach(pin);
}
bool ServoSpeed::set(int angle,int speed)
{
	if(flag)
	{
		target=angle;
		step=(target - angle) / abs(angle - target);
		flag=false;
	}
}
int ServoSpeed::nowangle()
{
	return nowdeg;
}
void ServoSpeed::update()
{
	nowdeg = (angle - MIN_PULSE) / Step;
	if (st.stand_by(1.f / (float)speed) && !flag)
		angle += step;
	if (angle == target||angle>=max||angle<=min)
		flag = true;
	s.writeMicroseconds(angle);
}

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
	ESPinit(); //ESP32mother Initialize
	Servobegin();
	/*task setup*/
	run.setfunction("ROBOCON MAIN task", MAIN);
	run.setfunction("Servo task", Servotest);
	Start_();
}

void loop()
{
	ESPUpdate();
}

uint8_t ST[4];
void MAIN()
{
	if (StartFlag)
	{
		MELODY = true;
		SerialMonitor = true;
		StartFlag = false;
	}
	ST[0] = (PS3stick(lX)>0)?128-PS3stick(lX):255-PS3stick(lX)+128;
	ST[1] = (PS3stick(lY)>0)?128-PS3stick(lY):255-PS3stick(lY)+128;
	ST[2] = (PS3stick(rX)>0)?128-PS3stick(rX):255-PS3stick(rX)+128;
	ST[3] = (PS3stick(rY)*-1>0)?128-PS3stick(rY)*-1:255-PS3stick(rY)*-1+128;
	if (debug_t)
	{
		ESP_SERIAL.printf("%f/%f/%f/lx=%d/ly=%d/rx=%d/ry=%d\n\r",Vx,Vy,Angular,ST[0],ST[1],ST[2],ST[3]);
		ESP_SERIAL.print(PS3Debug);
	}
	if (EMARGENCYSTOP())
		return;
	PS3Controller(&Vx,&Vy,&Angular,false);
	for(int i=0;i<4;i++)
	{
		angle[i]=map(ST[i],0,255,MIN_PULSE,MAX_PULSE);
		s[i].writeMicroseconds(angle[i]);
	}
    wheel->Move(Vy,Vx,Angular+Vx);
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
		for(int i=0;i<8;i++)
			ESP_SERIAL.printf("(%d,%d,%d)",angle[i],target[i],STEP[i]);
		ESP_SERIAL.printf("\n\r");
	}
	if (EMARGENCYSTOP())
		return;
	if (Dt.stand_by(0.5))
	{
		int nowdeg = 0;
		for (int i = 0; i < 4; i++)
		{
			Speed[0] = 100;
			Speed[1] = 100;
			Speed[2] = 100;
			Speed[3] = 100;
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