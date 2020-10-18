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

void MAIN();

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
	run.setfunction("ROBOCON MAIN task",MAIN);
	Start_();
}

void loop()
{
	ESPUpdate();
}

void MAIN()
{
    if(StartFlag)
    {
        MELODY = true;
        SerialMonitor = true;
        StartFlag=false;
    }
	if (debug_t)
	{
		ESP_SERIAL.printf("Main\n\r");
	}
	if (EMARGENCYSTOP())
		return;
}