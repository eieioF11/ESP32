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
#include "Trapezoid.h"
//#include "Web.h"

void Linetrace(Flag_t *flag);
void angletest(Flag_t *flag);
void movetest(Flag_t *flag);
void Lidertest(Flag_t *flag);
//void Webtest(Flag_t *flag);

Angle_correction acorrX(-0.035, -0.0005, -0.01, -1.0, 1.0, Delta_T);
Angle_correction acorrY(-0.035, -0.005, -0.01, -1.0, 1.0, Delta_T);
Angle_correction acorrZ(-0.0035, -0.0001, -0.001, -1.0, 1.0, Delta_T);

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
	//WebSetup();
	/*task setup*/
	//ESP32M.setfunction("Linetrace", Linetrace);
	ESP32M.setfunction("movetest", movetest);
	ESP32M.setfunction("angletest", angletest);
	//ESP32M.setfunction("lidertest", Lidertest);
	//ESP32M.setfunction("Webtest", Webtest);

	acorrX.setup(0);
	acorrY.setup(0);
	acorrZ.setup(0);
}

void loop()
{
	ESP32M.update();
}

PID ml(-0.019, -0.0, -0.0000001, 0.2, 0.6, Delta_T);
PID mr(-0.019, -0.0, -0.0000001, 0.2, 0.6, Delta_T);
PID ml2(-0.055, 0, 0, -0.25, 0.6, Delta_T);
PID mr2(-0.055, 0, 0, -0.25, 0.6, Delta_T);
void Linetrace(Flag_t *flag) //23~70
{
    if(flag->Start)
    {
        flag->Melody = true;
        flag->SerialMonitor = true;
        flag->Start=false;
		Vx=Vy=Angular=0;
    }
	const float lv = 38.0;
	const float lv2 = 50.0;
	/*Linetrace*/
	float lmp = ml.output(lv, l1.read(3));
	float rmp = mr.output(lv, l1.read(1));
	float lm2p = ml2.output(lv2, l1.read(4));
	float rm2p = mr2.output(lv2, l1.read(0));
	if (flag->Debug)
	{
		ESP_SERIAL.printf("%.2f,%.2f,%.2f,%.2f/l %.2f/r %.2f(/%d/%d/%d/%d/%d/sw%d%d\n\r", lmp, rmp, lm2p, rm2p, lmp + lm2p, rmp + rm2p, l1.read(4), l1.read(3), l1.read(2), l1.read(1), l1.read(0), l1.swread(sw1), l1.swread(sw2));
	}
	if (ESP32M.EMARGENCYSTOP())
		return;
	if (l1.swread(sw1) || l1.swread(sw2))
		wheel->Stop();
	else
	{
		((ESPMotor *)md[0])->Move((lmp + lm2p > 0) ? lmp + lm2p : 0);
		((ESPMotor *)md[1])->Move((rmp + rm2p > 0) ? rmp + rm2p : 0);
	}
}

Timer t[1];
void angletest(Flag_t *flag)
{
	bool reset = false;
    if(flag->Start)
    {
        flag->Melody = true;
        flag->SerialMonitor = true;
        flag->Start=false;
		Vx=Vy=Angular=0;
    }
	switch (flag->SerialData)
	{
	case 'r':
		reset = true;
		break;
	}
	if (!mount())
	{
		reset = true;
		ESP32M.Start();
	}
	if (flag->Debug)
	{
		//ESP_SERIAL.printf("rps1=%5.3f,rps2=%5.3f,pitch=%5.2f/roll=%5.2f/yaw=%5.2f//(x=%5.2f,y=%5.2f,angle=%5.2f)/%5.3f/%5.3f,%5.3f,%5.3f,reset%d,Gz %f\n\r", md[0]->now_val, md[1]->now_val, odm.pitch(), odm.roll(), odm.yaw(), odm.x(), odm.y(), odm.wyaw(), odm.V(), Vx, Vy, Angular, reset, mpu.read(GyroZ));
		ESP_SERIAL.printf("/roll=%5.2f/pitch=%5.2f/yaw=%5.2f/%5.2f/%5.2f\n\r",odm.roll(),odm.pitch(),odm.yaw(),odm.wyaw(),Angular);
		//mpu.print();
	}
	if (ESP32M.EMARGENCYSTOP())
		return;
	//Vx=acorrX.Output(reset,odm.pitch());
	//Vy = acorrY.Output(reset, odm.roll());
	Angular=acorrZ.Output(reset,odm.yaw());
	if ((l1.swread(sw1) && l1.swread(sw2) && Vy > 0))
		wheel->Stop();
	else
		wheel->Move(Vy, Vx, Angular);
}
void movetest(Flag_t *flag)
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

byte data[14];

void Lidertest(Flag_t *flag)
{
	uint16_t dis=(uint16_t)lider.read(Distance);
	float  angle=lider.read(Angle);
    if(flag->Start)
    {
        flag->Melody = true;
        flag->SerialMonitor = false;
		ESP32M.Start();
        flag->Start=false;
		Vx=Vy=Angular=0;
    }
	else
	{
		data[0]=dis>>8;
		data[1]=dis&0xff;
		uint16_t Deg=(uint16_t)angle;
		data[2]=Deg>>8;
		data[3]=Deg&0xff;
		Serial.write(data,4);
	}
	if (flag->Debug)
	{
		ESP_SERIAL.printf("dis=%f,angle=%f\n\r",(float)dis,angle);
	}
	if (ESP32M.EMARGENCYSTOP())
		return;
}

/*
WebObject<uint8_t> botton(1);
WebObject<int8_t> stX(2);
WebObject<int8_t> stY(3);
WebObject<uint8_t> EM(4);
WebObject<uint8_t> checkbox1(5);
void Webtest(Flag_t *flag)
{
    if(StartFlag)
    {
        MELODY = true;
        SerialMonitor = true;
        StartFlag=false;
    }
	Vy = (float)stY / 128.f;
	Angular = (float)stX / 128.f;
	if (debug_t)
	{
		ESP_SERIAL.printf("checkbox %d,botton %d,%d,(%d,%d),Vx=%f,Vy=%f,Angler=%f\n\r", (uint8_t)checkbox1, (uint8_t)botton, (uint8_t)EM, (int8_t)stX, (int8_t)stY, Vx, Vy, Angular);
	}
	if (EMARGENCYSTOP() || (uint8_t)EM)
	{
		digitalWrite(STBY, LOW);
		return;
	}
	wheel->Move(Vy, Vx, Angular);
}*/