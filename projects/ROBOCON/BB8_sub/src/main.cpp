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
const int anglemap[][2][4]=
{
	{
		{10,120,110,120},
		{10,  0,110,  0},
	},
	{
		{120,0,120,0},
		{  0,0,120,0},
	},
	{
		{40,0,100,120},
		{40,0,100,120},
	},
	{
		{40,40,100,80},
		{40,40,100,80},
	},
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

void setup()
{
	odm.setup(TWOWHEEL);
	ESPinit(); //ESP32mother Initialize
	Servobegin();
	/*User task setup*/
	ESP32M.setfunction("ROBOCON MAIN task", MAIN);
	xTaskCreatePinnedToCore(Servo,"servo task",1024,NULL,3,NULL,1);
}

void loop()
{
	ESP32M.update();
}

uint8_t ST[4];
uint8_t count[4];
uint8_t movesel=0;
uint8_t Servoend[4]={0};
bool walk=false;
bool wflag=false;
void MAIN(Flag_t *flag)
{
    if(flag->Start)
    {
        flag->Melody = true;
        flag->SerialMonitor = true;
        flag->Start=false;
    }
	ST[0] = (PS3stick(lX)>0)?128-PS3stick(lX):255-PS3stick(lX)+128;
	ST[1] = (PS3stick(lY)*-1>0)?128-PS3stick(lY)*-1:255-PS3stick(lY)*-1+128;
	ST[2] = (PS3stick(rX)>0)?128-PS3stick(rX):255-PS3stick(rX)+128;
	ST[3] = (PS3stick(rY)>0)?128-PS3stick(rY):255-PS3stick(rY)+128;
	if (flag->Debug)
	{
		ESP_SERIAL.printf("%f/%f/%f/lx=%d/ly=%d/rx=%d/ry=%d\n\r",Vx,Vy,Angular,ST[0],ST[1],ST[2],ST[3]);
		ESP_SERIAL.print(PS3Debug);
	}
	if (ESP32M.EMARGENCYSTOP())
		return;
	uint16_t PS3button=PS3Button();
    PS3Debug="";
    for(int i=0;i<16;i++)
    {
        switch((int)PS3button&1ull<<i)//0b/select/start/l3/r3/l1/r1/l2/r2/up/down/right/left/△/〇/×/□/
        {
            case 0b1000000000000000:St.addprintf(&PS3Debug,"select\n");break;
            case 0b0100000000000000:St.addprintf(&PS3Debug,"start\n");break;
            case 0b0010000000000000:St.addprintf(&PS3Debug,"l3\n");break;
            case 0b0001000000000000:St.addprintf(&PS3Debug,"r3\n");break;
            case 0b0000100000000000:St.addprintf(&PS3Debug,"l1\n");Angular=0.2;break;
            case 0b0000010000000000:St.addprintf(&PS3Debug,"r1\n");Angular=-0.2;break;
            case 0b0000001000000000:St.addprintf(&PS3Debug,"l2\n");break;
            case 0b0000000100000000:St.addprintf(&PS3Debug,"r2\n");break;
            case 0b0000000010000000:St.addprintf(&PS3Debug,"up\n");if(movesel!=2&&movesel!=3)movesel=0;walk=true;Vy=-0.2;break;
            case 0b0000000001000000:St.addprintf(&PS3Debug,"down\n");if(movesel!=2&&movesel!=3)movesel=0;walk=true;Vy=0.2;break;
            case 0b0000000000100000:St.addprintf(&PS3Debug,"right\n");Angular=0.2;break;
            case 0b0000000000010000:St.addprintf(&PS3Debug,"left\n");Angular=-0.2;break;
            case 0b0000000000001000:St.addprintf(&PS3Debug,"triangle\n");movesel=1;wflag=false;break;
            case 0b0000000000000100:St.addprintf(&PS3Debug,"circle\n");movesel=2;wflag=false;break;
            case 0b0000000000000010:St.addprintf(&PS3Debug,"cross\n");movesel=3;wflag=false;break;
            case 0b0000000000000001:St.addprintf(&PS3Debug,"square\n");break;
        }
    }
    if(!PS3button)
        Vx=Vy=Angular=0.0f;
	if(walk)
	{
		if(!wflag)
		{
			for(int i=0;i<4;i++)
			{
				Servoend[i]=0;
				count[i]=0;
			}
			wflag=true;
		}
	}
	//PS3Controller(&Vx,&Vy,&Angular,false);
	for(int i=0;i<4;i++)
	{
		if(PS3button||movesel==2||movesel==3)
		{
			if(s[i].set(anglemap[movesel][count[i]][i],10000,2))
			{
				if(movesel!=0)
					count[i]=(count[i]>=1)?0:count[i]+1;
				else
				{
					Servoend[i]=1;
				}
			}
			if(Servoend[0]==1&&Servoend[1]==1&&Servoend[2]==1&&Servoend[3]==1)
			{
				for(int j=0;j<4;j++)
				{
					Servoend[j]=0;
					i=0;
					count[j]=(count[j]>=1)?0:count[j]+1;
				}
			}
		}
		else
		{
			wflag=false;
			angle[i]=map(ST[i],0,255,MIN_PULSE,MAX_PULSE);
			s[i].setMs(angle[i]);
		}
	}
    wheel->Move(Vy,Vx,Angular);
}