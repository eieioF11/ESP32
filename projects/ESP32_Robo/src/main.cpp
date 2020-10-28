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

void Linetrace();
void angletest();
void movetest();
void Lidertest();
//void Webtest();

Angle_correction acorrX(-0.035, -0.0005, -0.01, -1.0, 1.0, Delta_T);
Angle_correction acorrY(-0.035, -0.005, -0.01, -1.0, 1.0, Delta_T);
Angle_correction acorrZ(-0.0035, -0.0001, -0.001, -1.0, 1.0, Delta_T);

Trapezoid T(&odm);

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
	//WebSetup();
	/*task setup*/
	run.setfunction("Linetrace", Linetrace);
	run.setfunction("angletest", angletest);
	run.setfunction("movetest", movetest);
	run.setfunction("lidertest", Lidertest);
	//run.setfunction("Webtest", Webtest);

	acorrX.setup(0);
	acorrY.setup(0);
	acorrZ.setup(0);
	//Start_();
}

void loop()
{
	ESPUpdate();
}

PID ml(-0.019, -0.0, -0.0000001, 0.2, 0.6, Delta_T);
PID mr(-0.019, -0.0, -0.0000001, 0.2, 0.6, Delta_T);
PID ml2(-0.055, 0, 0, -0.25, 0.6, Delta_T);
PID mr2(-0.055, 0, 0, -0.25, 0.6, Delta_T);
void Linetrace() //23~70
{
    if(StartFlag)
    {
        MELODY = true;
        SerialMonitor = true;
        StartFlag=false;
    }
	const float lv = 38.0;
	const float lv2 = 50.0;
	/*Linetrace*/
	float lmp = ml.output(lv, l1.read(3));
	float rmp = mr.output(lv, l1.read(1));
	float lm2p = ml2.output(lv2, l1.read(4));
	float rm2p = mr2.output(lv2, l1.read(0));
	if (debug_t)
	{
		ESP_SERIAL.printf("%.2f,%.2f,%.2f,%.2f/l %.2f/r %.2f(/%d/%d/%d/%d/%d/sw%d%d\n\r", lmp, rmp, lm2p, rm2p, lmp + lm2p, rmp + rm2p, l1.read(4), l1.read(3), l1.read(2), l1.read(1), l1.read(0), l1.swread(sw1), l1.swread(sw2));
	}
	if (EMARGENCYSTOP())
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
void angletest()
{
	bool reset = false;
#if (ESP_MODE == ESP_PS3)
	//Angular=acorr.Output(reset);
	bool b = PS3Controller(&Vx, &Vy, &Angular);
	if (b || (l1.swread(sw1) && l1.swread(sw2) && Vy > 0))
		wheel->Stop();
	else
		wheel->Move(Vy, Vx, Angular);
	switch (serialdata)
	{
	case 'r':
		reset = true;
		break;
	}
	if (reset)
	{
		odm.reset();
	}
	if (debug_t)
	{
		ESP_SERIAL.printf("rps1=%5.3f,rps2=%5.3f,pitch=%5.2f/roll=%5.2f/yaw=%5.2f//(x=%5.2f,y=%5.2f,angle=%5.2f)/%5.2f\n\r", md[0]->now_val, md[1]->now_val, odm.pitch(), odm.roll(), odm.yaw(), odm.x(), odm.y(), odm.wyaw(), odm.V());
		//ESP_SERIAL.printf("sp=%f/angler=%f/",sp,angler);
		//mpu.print();
	}
#else
    if(StartFlag)
    {
        MELODY = true;
        SerialMonitor = true;
        StartFlag=false;
    }
	switch (serialdata)
	{
	case 'r':
		reset = true;
		break;
	}
	if (!mount())
	{
		reset = true;
		Start();
	}
	if (debug_t)
	{
		//ESP_SERIAL.printf("rps1=%5.3f,rps2=%5.3f,pitch=%5.2f/roll=%5.2f/yaw=%5.2f//(x=%5.2f,y=%5.2f,angle=%5.2f)/%5.3f/%5.3f,%5.3f,%5.3f,reset%d,Gz %f\n\r", md[0]->now_val, md[1]->now_val, odm.pitch(), odm.roll(), odm.yaw(), odm.x(), odm.y(), odm.wyaw(), odm.V(), Vx, Vy, Angular, reset, mpu.read(GyroZ));
		ESP_SERIAL.printf("/roll=%5.2f/pitch=%5.2f/yaw=%5.2f/%5.2f/%5.2f\n\r",odm.roll(),odm.pitch(),odm.yaw(),odm.wyaw(),Angular);
		//mpu.print();
	}
	if (EMARGENCYSTOP())
		return;
	//Vx=acorrX.Output(reset,odm.pitch());
	//Vy = acorrY.Output(reset, odm.roll());
	Angular=acorrZ.Output(reset,odm.yaw());
	if ((l1.swread(sw1) && l1.swread(sw2) && Vy > 0))
		wheel->Stop();
	else
		wheel->Move(Vy, Vx, Angular);
#endif
}
void movetest()
{
    if(StartFlag)
    {
        MELODY = true;
        SerialMonitor = true;
        StartFlag=false;
    }
	static bool st = false;
	if (debug_t)
	{
		ESP_SERIAL.print(T.status());
		ESP_SERIAL.printf("/Vx=%f,Vy=%f,Angler=%f,rpm=%7.3f,rpm=%7.3f/(x=%5.2f,y=%5.2f,angle=%5.2f)\n\r", Vx, Vy, Angular, md[0]->now_val, md[1]->now_val, odm.x(), odm.y(), odm.wyaw());
	}
	if (EMARGENCYSTOP())
		return;
	switch (serialdata)
	{
	case 's':
		st = true;
		break;
	}
	if (st)
	{
		if (T.turn(-90, 0.02))
			st = false;
		//if(T.movepoint(100.0*m,-100.0*m,0.02))
		//st=false;
	}
	T.update(&Vx, &Vy, &Angular);
	wheel->Move(Vy, Vx, Angular);
}

byte data[14];

void Lidertest()
{
	uint16_t dis=(uint16_t)lider.read(Distance);
	float  angle=lider.read(Angle);
    if(StartFlag)
    {
        MELODY = true;
        SerialMonitor = false;
		Start();
        StartFlag=false;
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
	if (debug_t)
	{
		ESP_SERIAL.printf("dis=%f,angle=%f\n\r",(float)dis,angle);
	}
	if (EMARGENCYSTOP())
		return;
}

/*
WebObject<uint8_t> botton(1);
WebObject<int8_t> stX(2);
WebObject<int8_t> stY(3);
WebObject<uint8_t> EM(4);
WebObject<uint8_t> checkbox1(5);
void Webtest()
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