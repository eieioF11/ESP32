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
#include <Arduino.h>
#include "ESP32mother.h"

void Main(Flag_t *flag);

Trapezoid T(&odm);

void setup()
{
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
	ESP32M.setfunction("main", Main);
}

void loop()
{
	ESP32M.update();
}

void Main(Flag_t *flag)
{
    if(flag->Start)
    {
        flag->Melody = false;
        flag->SerialMonitor = true;
        flag->Start=false;
		Vx=Vy=Angular=0;
    }
	static bool st = false;
	if (flag->Debug)
	{
		ESP_SERIAL.print(T.status());
		//ESP_SERIAL.printf("/Vx=%f,Vy=%f,Angler=%f,rpm=%7.3f,rpm=%7.3f/(x=%5.2f,y=%5.2f,angle=%5.2f)/roll=%5.2f/pitch=%5.2f/yaw=%5.2f/\n\r", Vx, Vy, Angular, md[0]->now_val, md[1]->now_val, odm.x(ODOM_mm), odm.y(ODOM_mm), odm.wyaw(),odm.roll(),odm.pitch(),odm.yaw());
		ESP_SERIAL.printf("/Vx=%f,Vy=%f,Angler=%f,rpm=%7.3f,rpm=%7.3f/(x=%5.2f,y=%5.2f,angle=%5.2f)\n\r", Vx, Vy, Angular, md[0]->now_val, md[1]->now_val, odm.x(ODOM_mm), odm.y(ODOM_mm), odm.wyaw());
	}
	if (ESP32M.EMARGENCYSTOP())
		return;
	if(!mount())
	{
		ESP32M.Start();
		st = true;
	}
	switch (flag->SerialData)
	{
        case 'w':Vy+=0.1;break;
        case 's':Vy-=0.1;break;
        case 'a':Angular+=0.1;break;
        case 'd':Angular-=0.1;break;
		case 'S':st = true;break;
		case 'r':odm.reset();break;
	}
	if (st)
	{
		if(T.movepoint(500.0*ODOM_m,500.0*ODOM_m,0.2))
		{
			st=false;
			ESP32M.Start_();
		}
		T.update(&Vx, &Vy, &Angular);
	}
	wheel->Move(Vy, Vx, Angular);
}