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
#include "ServoSpeed.h"

volatile int angle[4];
const int anglemap[2][4]=
{
	{0,0,0,10},
	{90,90,90,10},
};

ServoSpeed s[4];
//Servo s[4];
const int servopin[4] = {enc1a, enc1b, enc2a, enc2b};

void Servobegin()
{
	for (int i = 0; i < 4; i++)
		s[i].begin(servopin[i],MIN_PULSE,MAX_PULSE);
}
void Servomove()
{
	for (int i = 0; i < 4; i++)
		s[i].update();
}

void Servo(void *arg)
{
    portTickType lt = xTaskGetTickCount();
    while(1)
    {
		if(!ESP32M.EMARGENCYSTOP())
            Servomove();
		vTaskDelayUntil(&lt, 1/portTICK_RATE_MS);
    }
}

void MAIN(Flag_t *flag);
void Servotest(Flag_t *flag);

void setup()
{
	odm.setup(TWOWHEEL);
	ESPinit(); //ESP32mother Initialize
	Servobegin();
	/*User task setup*/
	ESP32M.setfunction("ROBOCON MAIN task", MAIN);
	ESP32M.setfunction("Servo task", Servotest);
	xTaskCreatePinnedToCore(Servo,"servo task",1024,NULL,2,NULL,0);
}

void loop()
{
	ESP32M.update();
}

uint8_t ST[4];
void MAIN(Flag_t *flag)
{
    if(flag->Start)
    {
        flag->Melody = true;
        flag->SerialMonitor = true;
        flag->Start=false;
    }
	ST[0] = (PS3stick(lX)>0)?128-PS3stick(lX):255-PS3stick(lX)+128;
	ST[1] = (PS3stick(lY)>0)?128-PS3stick(lY):255-PS3stick(lY)+128;
	ST[2] = (PS3stick(rX)>0)?128-PS3stick(rX):255-PS3stick(rX)+128;
	ST[3] = (PS3stick(rY)*-1>0)?128-PS3stick(rY)*-1:255-PS3stick(rY)*-1+128;
	if (flag->Debug)
	{
		ESP_SERIAL.printf("%f/%f/%f/lx=%d/ly=%d/rx=%d/ry=%d\n\r",Vx,Vy,Angular,ST[0],ST[1],ST[2],ST[3]);
		ESP_SERIAL.print(PS3Debug);
	}
	if (ESP32M.EMARGENCYSTOP())
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

uint8_t count[4];
void Servotest(Flag_t *flag)
{
    if(flag->Start)
    {
        flag->Melody = true;
        flag->SerialMonitor = true;
        flag->Start=false;
    }
	if (flag->Debug)
	{
		for(int i=0;i<4;i++)
			ESP_SERIAL.printf("/%f,%d,%d/",s[i].nowangle(),s[i].nowMs(),count[i]);
		ESP_SERIAL.printf("\n\r");
	}
	if (ESP32M.EMARGENCYSTOP())
		return;
	for (int i = 0; i < 4; i++)
	{
		if(s[i].set(anglemap[count[i]][i],7000))
		{
			count[i]=(count[i]>=1)?0:count[i]+1;
		}
	}
	//Servomove();
}