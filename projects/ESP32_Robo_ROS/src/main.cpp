#include <Arduino.h>
#include "ESP32mother.h"

void Main(Flag_t *flag);

Trapezoid T(&odm);

void setup()
{
	ESPtask_i2c_init = []()
	{
		if (imu.begin() != INV_SUCCESS)
		{
			while (1)
			{
				Serial.println("Unable to communicate with MPU-9250");
				Serial.println("Check connections, and try again.");
				Serial.println();
				delay(5000);
			}
		}
		imu.dmpBegin(DMP_FEATURE_6X_LP_QUAT |  // Enable 6-axis quat
						 DMP_FEATURE_GYRO_CAL, // Use gyro calibration
					 10);					   // Set DMP FIFO rate to 10 Hz
											   //mpu.setup();
	}; //i2cを使用するものの初期化
	ESPtask_i2c_while = []() {
		static int ct = 0;
		switch (ct)
		{
		case 0:
			I2C_Error = (i2c_err_t)l1.com();
		case 1:
			// Check for new data in the FIFO
			if (imu.fifoAvailable())
			{
				// Use dmpUpdateFifo to update the ax, gx, mx, etc. values
				if (imu.dmpUpdateFifo() == INV_SUCCESS)
				{
					// computeEulerAngles can be used -- after updating the
					// quaternion values -- to estimate roll, pitch, and yaw
					imu.computeEulerAngles();
				}
			}
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
	//printf(",%f,%f,%f\n\r",odm.x(ODOM_m),odm.y(ODOM_m),odm.yaw());
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
	//if(Vy==0.f && Vx==0.f && Angular==0.0f)
		//b=PS3ControllerTw(&Vx,&Vy,&Angular);
	if(b||(l1.swread(sw1)&&l1.swread(sw2)&&Vx>0))
		wheel->Stop();
	else
		wheel->Move(Vx, Vy, Angular);
}