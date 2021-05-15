#include <Arduino.h>
#include "ESP32mother.h"

void Main(Flag_t *flag);

Trapezoid T(&odm);

void setup()
{
	ESPtask_i2c_init = []() {
		mpu.setup();
	}; //i2cを使用するものの初期化
	ESPtask_i2c_while = []() {
		static int ct = 0;
		switch (ct)
		{
		case 0:
			I2C_Error = (i2c_err_t)l1.com();
		case 1:
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
        flag->SerialMonitor = false;
        flag->Start=false;
		Vx=Vy=Angular=0;
    }
	float BTL=BatteryLevel();
	//printf("%f[v],%f,%f,%f\n\r",Vmonitor(),odm.x(ODOM_m),odm.y(ODOM_m),odm.wyaw());
	//printf("%f[v],%f[%]\n\r",Vmonitor(),BTL);
	ROS_EMARGENCYSTOP=(bool)ESP32M.EMARGENCYSTOP();
	if(BTL<LowLevel&&BTL!=0.f)
		ESP32M.BTError();
	if (ROS_EMARGENCYSTOP)
		return;
	Vy = (float)Tw_Vy;
	Vx = (float)Tw_Vx;
	Angular = (float)Tw_Angular;
	bool b=false;
	if(Vy==0.f && Vx==0.f && Angular==0.0f)
		b=PS3ControllerTw(&Vx,&Vy,&Angular);
	if(b||(l1.swread(sw1)&&l1.swread(sw2)&&Vx>0))
		wheel->Stop();
	else
		wheel->Move(Vx, Vy, Angular);
}