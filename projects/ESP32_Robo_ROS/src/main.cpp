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
	ROS_EMARGENCYSTOP=(bool)ESP32M.EMARGENCYSTOP();
	if (ROS_EMARGENCYSTOP)
		return;
	Vy = T.vtorps(Tw_Vy);
	Vx = T.vtorps(Tw_Vx);
	Angular = T.vtorps(Tw_Angular);
	wheel->Move(Vx, Vy, Angular);
}