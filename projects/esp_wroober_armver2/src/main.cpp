#include <Arduino.h>
#include "ESP32mother.h"
#include "ESP32Servo.h"
#define MIN_PULSE 500
#define MAX_PULSE 2400
#define Step (MAX_PULSE - MIN_PULSE) / 180

Servo s[4];
Timer st[4],dt;
const int servopin[4] = {18,19,21,22};
void Servobegin()
{
	for (int i = 0; i < 4; i++)
	{
		s[i].setPeriodHertz(50);
		s[i].attach(servopin[i]);
	}
}
void setup()
{
	Serial.begin(115200);
	Servobegin();
	PS3init();
	Serial.println("End setup");
}

volatile int angle[4];
uint8_t ST[4];
bool hold=false;
Chattering HOLD(3,120);
void PS3Controller()
{
    PS3Debug="";
	uint16_t PS3button=PS3Button();
    for(int i=0;i<16;i++)
    {
        switch((int)PS3button&1ull<<i)//0b/select/start/l3/r3/l1/r1/l2/r2/up/down/right/left/△/〇/×/□/
        {
            case 0b1000000000000000:St.addprintf(&PS3Debug,"select\n");break;
            case 0b0100000000000000:St.addprintf(&PS3Debug,"start\n");break;
            case 0b0010000000000000:St.addprintf(&PS3Debug,"l3\n");break;
            case 0b0001000000000000:St.addprintf(&PS3Debug,"r3\n");break;
            case 0b0000100000000000:St.addprintf(&PS3Debug,"l1\n");break;
            case 0b0000010000000000:St.addprintf(&PS3Debug,"r1\n");break;
            case 0b0000001000000000:St.addprintf(&PS3Debug,"l2\n");HOLD<=1;break;
            case 0b0000000100000000:St.addprintf(&PS3Debug,"r2\n");break;
            case 0b0000000010000000:St.addprintf(&PS3Debug,"up\n");break;
            case 0b0000000001000000:St.addprintf(&PS3Debug,"down\n");break;
            case 0b0000000000100000:St.addprintf(&PS3Debug,"right\n");break;
            case 0b0000000000010000:St.addprintf(&PS3Debug,"left\n");break;
            case 0b0000000000001000:St.addprintf(&PS3Debug,"triangle\n");break;
            case 0b0000000000000100:St.addprintf(&PS3Debug,"circle\n");break;
            case 0b0000000000000010:St.addprintf(&PS3Debug,"cross\n");break;
            case 0b0000000000000001:St.addprintf(&PS3Debug,"square\n");break;
			default:break;
        }
    }
	if(HOLD==1)
		hold=!hold;
	if(hold)
		ST[3]=1;
	else
		ST[3]=75;
	HOLD<=0;
	ST[0] = (PS3stick(lX)>0)?128-PS3stick(lX):255-PS3stick(lX)+128;
	ST[1] = (PS3stick(lY)>0)?128-PS3stick(lY):255-PS3stick(lY)+128;
	ST[2] = (PS3stick(rY)>0)?128-PS3stick(rY):255-PS3stick(rY)+128;
	//ST[3] = (PS3stick(rX)>0)?128-PS3stick(rX):255-PS3stick(rX)+128;
}
void loop()
{
	PS3Controller();
	if (dt.stand_by(0.5))
	{
		ESP_SERIAL.printf("%f/%f/%f/lx=%d/ly=%d/rx=%d/ry=%d,hold %d\n\r",Vx,Vy,Angular,ST[0],ST[1],ST[2],ST[3],hold);
		ESP_SERIAL.print(PS3Debug);
	}

	for(int i=0;i<4;i++)
	{
		angle[i]=map(ST[i],0,255,MIN_PULSE,MAX_PULSE);
		s[i].writeMicroseconds(angle[i]);
	}
	PS3Update();
	delay(20);
}