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
#define MAX_ANGLE 120
#define Step (MAX_PULSE - MIN_PULSE) / MAX_ANGLE

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
		bool stop;
		bool start;
	public:
		ServoSpeed();
		void begin(int pin,int min,int max);
		bool set(int angle,int speed);
		void setMs(int val);
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
	flag = false;
	start = true;
	s.setPeriodHertz(50);
	s.attach(pin);
}
bool ServoSpeed::set(int angle,int speed)
{
	if(this->start)
	{
		this->target=map(angle,0,MAX_ANGLE,this->min,this->max);
		int S=abs(this->angle - this->target);
		if(S!=0)
			this->step=(this->target - this->angle) / S;
		this->speed=speed;
		this->flag=false;
		this->start=false;
		this->stop=false;
	}
	if(this->flag)
	{
		this->start=true;
		this->flag=false;
		return true;
	}
	return false;
}
void ServoSpeed::setMs(int val)
{
	this->stop=true;
	s.writeMicroseconds(val);
}
int ServoSpeed::nowangle()
{
	return this->nowdeg;
}
void ServoSpeed::update()
{
	nowdeg = (angle - MIN_PULSE) / Step;
	if (st.stand_by(1.f / (float)speed) && !flag)
		angle += step;
	if (angle == target||angle>=max||angle<=min)
		flag = true;
	if(!stop)
		s.writeMicroseconds(angle);
}

volatile int angle[4];
const int anglemap[2][4]=
	{
		{0,90,0,0},
		{90,0,0,0},
	};

/*
volatile int target[4];
volatile int Speed[4];
volatile int8_t STEP[4];
volatile bool END[4];
*/
ServoSpeed s[4];
//Servo s[4];
Timer st[4], Dt;
const int servopin[4] = {enc1a, enc1b, enc2a, enc2b};

void Servobegin()
{
	for (int i = 0; i < 4; i++)
	{
		s[i].begin(servopin[i],MIN_PULSE,MAX_PULSE);
	}
}
void Servomove()
{
	for (int i = 0; i < 4; i++)
	{
		s[i].update();
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
		//s[i].writeMicroseconds(angle[i]);
		s[i].setMs(angle[i]);
	}
    wheel->Move(Vy,Vx,Angular+Vx);
}

uint8_t count;
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
		for(int i=0;i<4;i++)
			ESP_SERIAL.printf("%d,%d",angle[i],s[i].nowangle());
		ESP_SERIAL.printf("\n\r");
	}
	if (EMARGENCYSTOP())
		return;
	if (Dt.stand_by(0.5))
	{
		int nowdeg = 0;
		for (int i = 0; i < 4; i++)
		{
			if(s[i].set(anglemap[count][i],1000))
			{
				count=(count>2)?0:count+1;
				ESP_SERIAL.printf("END\n\r");
			}
		}
	}
	Servomove();
}