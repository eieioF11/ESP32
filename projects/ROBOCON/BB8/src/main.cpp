#include <Arduino.h>
#include "Setup_ROBOCON.h"
#include <SPI.h>
#include <Wire.h>
#include "ROBO_MD.h"
#include "MoveBase.h"
#include "Omuni4.h"
#include "odometry.h"
#include "Filtered_double.h"
#include "Timer.h"
#include "stprint.h"

#include "Ps3Controller.h"
#define MAC_ADD (char *)"98:F4:AB:1B:70:8E"

void PS3init(); //setup()で１回実行
void PS3Update();
bool PS3Controller(); //-1~1

bool conect = false;
uint16_t PS3button = 0b0000000000000000; //0b/select/start/l3/r3/l1/r1/l2/r2/up/down/right/left/△/〇/×/□/
bool psbt = false;
int8_t lx = 0, ly = 0, rx = 0, ry = 0;
String PS3Debug = "";

void notify()
{
	//---------- Digital select/start/ps button events ---------
	if (Ps3.event.button_down.select && !(PS3button & 1ull << 15))
		PS3button += 0b1000000000000000;
	;
	if (Ps3.event.button_up.select && (PS3button & 1ull << 15))
		PS3button -= 0b1000000000000000;

	if (Ps3.event.button_down.start && !(PS3button & 1ull << 14))
		PS3button += 0b0100000000000000;
	if (Ps3.event.button_up.start && (PS3button & 1ull << 14))
		PS3button -= 0b0100000000000000;

	if (Ps3.event.button_down.ps)
		psbt = true;
	if (Ps3.event.button_up.ps)
		psbt = false;

	//--------------- Digital stick button events --------------
	if (Ps3.event.button_down.l3 && !(PS3button & 1ull << 13))
		PS3button += 0b0010000000000000;
	if (Ps3.event.button_up.l3 && (PS3button & 1ull << 13))
		PS3button -= 0b0010000000000000;

	if (Ps3.event.button_down.r3 && !(PS3button & 1ull << 12))
		PS3button += 0b0001000000000000;
	if (Ps3.event.button_up.r3 && (PS3button & 1ull << 12))
		PS3button -= 0b0001000000000000;

	//------------- Digital shoulder button events -------------
	if (Ps3.event.button_down.l1 && !(PS3button & 1ull << 11))
		PS3button += 0b0000100000000000;
	if (Ps3.event.button_up.l1 && (PS3button & 1ull << 11))
		PS3button -= 0b0000100000000000;

	if (Ps3.event.button_down.r1 && !(PS3button & 1ull << 10))
		PS3button += 0b0000010000000000;
	if (Ps3.event.button_up.r1 && (PS3button & 1ull << 10))
		PS3button -= 0b0000010000000000;

	//-------------- Digital trigger button events -------------
	if (Ps3.event.button_down.l2 && !(PS3button & 1ull << 9))
		PS3button += 0b0000001000000000;
	if (Ps3.event.button_up.l2 && (PS3button & 1ull << 9))
		PS3button -= 0b0000001000000000;

	if (Ps3.event.button_down.r2 && !(PS3button & 1ull << 8))
		PS3button += 0b0000000100000000;
	if (Ps3.event.button_up.r2 && (PS3button & 1ull << 8))
		PS3button -= 0b0000000100000000;

	//--------------- Digital D-pad button events --------------
	if (Ps3.event.button_down.up && !(PS3button & 1ull << 7))
		PS3button += 0b0000000010000000;
	if (Ps3.event.button_up.up && (PS3button & 1ull << 7))
		PS3button -= 0b0000000010000000;

	if (Ps3.event.button_down.down && !(PS3button & 1ull << 6))
		PS3button += 0b0000000001000000;
	if (Ps3.event.button_up.down && (PS3button & 1ull << 6))
		PS3button -= 0b0000000001000000;

	if (Ps3.event.button_down.right && !(PS3button & 1ull << 5))
		PS3button += 0b0000000000100000;
	if (Ps3.event.button_up.right && (PS3button & 1ull << 5))
		PS3button -= 0b0000000000100000;

	if (Ps3.event.button_down.left && !(PS3button & 1ull << 4))
		PS3button += 0b0000000000010000;
	if (Ps3.event.button_up.left && (PS3button & 1ull << 4))
		PS3button -= 0b0000000000010000;

	//--- Digital cross/square/triangle/circle button events ---
	if (Ps3.event.button_down.triangle && !(PS3button & 1ull << 3))
		PS3button += 0b0000000000001000;
	if (Ps3.event.button_up.triangle && (PS3button & 1ull << 3))
		PS3button -= 0b0000000000001000;

	if (Ps3.event.button_down.circle && !(PS3button & 1ull << 2))
		PS3button += 0b0000000000000100;
	if (Ps3.event.button_up.circle && (PS3button & 1ull << 2))
		PS3button -= 0b0000000000000100;

	if (Ps3.event.button_down.cross && !(PS3button & 1ull << 1))
		PS3button += 0b0000000000000010;
	if (Ps3.event.button_up.cross && (PS3button & 1ull << 1))
		PS3button -= 0b0000000000000010;

	if (Ps3.event.button_down.square && !(PS3button & 1ull << 0))
		PS3button += 0b0000000000000001;
	if (Ps3.event.button_up.square && (PS3button & 1ull << 0))
		PS3button -= 0b0000000000000001;

	//---------------- Analog stick value events ---------------
	if (abs(Ps3.event.analog_changed.stick.lx) + abs(Ps3.event.analog_changed.stick.ly) > 2)
	{
		lx = Ps3.data.analog.stick.lx;
		ly = Ps3.data.analog.stick.ly;
	}

	if (abs(Ps3.event.analog_changed.stick.rx) + abs(Ps3.event.analog_changed.stick.ry) > 2)
	{
		rx = Ps3.data.analog.stick.rx;
		ry = Ps3.data.analog.stick.ry;
	}
}

void onConnect()
{
	conect = true;
	Serial.println("Connected.");
}

void PS3init()
{
	Ps3.attach(notify);
	Ps3.attachOnConnect(onConnect);
	Ps3.begin(MAC_ADD);
}
int player = 0;
Timer ut;
void PS3Update()
{
	if (!Ps3.isConnected())
		return;
	if (ut.stand_by(0.5f))
	{
		//-------------------- Player LEDs -------------------
		Ps3.setPlayer(player);
		player += 1;
		if (player > 10)
			player = 0;
	}
}

#define Accelerator 0.01

Timer STLEDT;
MD *md[4];
MoveBase *wheel;
Filtered_double Vx, Vy, Angular;
//double Vx, Vy, Angular;

void I2C_task(void *arg) //I2C communication function
{
	pinMode(27, INPUT);//P2Bは設計ミスにより使用不可
    dacWrite(26, 32);//2A limit
	Wire.begin(ESP_SDA, ESP_SCL, 1000000);
	pinMode(ESP_SDA, PULLUP);
	pinMode(ESP_SCL, PULLUP);
	md[0] = new roboMD(0);
	md[1] = new roboMD(1);
	md[2] = new roboMD(2);
	md[3] = new roboMD(3);
	pinMode(STBY, OUTPUT);
	digitalWrite(STBY, LOW);

	Vx <<= Accelerator;
	Vy <<= Accelerator;
	Angular <<= Accelerator;

	wheel = new Omuni4(md);
	portTickType lt = xTaskGetTickCount();
	while (1)
	{
		digitalWrite(STBY, HIGH);
		PS3Controller();
		ESP_SERIAL.print(PS3Debug);
		wheel->Move((double)Vx, (double)Vy, (double)Angular);
		ESP_SERIAL.printf("/Vx=%3lf Vy=%3lf Angular=%3lf/%d/%d/%d/\n", (double)Vx, (double)Vy, (double)Angular,lx,ly,rx);
		vTaskDelayUntil(&lt, (int)(sensor_interval * 100) / portTICK_RATE_MS);
	}
}

void setup()
{
	Serial.begin(115200);
	//ESP_SERIAL.print("Booting...\n\r");
	xTaskCreatePinnedToCore(I2C_task, "I2C task", 8096, NULL, 1, NULL, 0);
	pinMode(StatusLED1, OUTPUT);
	pinMode(StatusLED2, OUTPUT);
	uint8_t baseMac[6];
	char baseMacChr[18] = {0};
	esp_read_mac(baseMac, ESP_MAC_BT);
	sprintf(baseMacChr, "%02X:%02X:%02X:%02X:%02X:%02X", baseMac[0], baseMac[1], baseMac[2], baseMac[3], baseMac[4], baseMac[5]);
	Serial.print("MAC: ");
	Serial.println(baseMacChr);
	digitalWrite(StatusLED1, LOW);
	digitalWrite(StatusLED2, HIGH);
	PS3init();
	digitalWrite(StatusLED2, LOW);
}

bool STLED = true;
void loop()
{
	if (STLEDT.stand_by(0.5))
		STLED = !STLED;
	digitalWrite(StatusLED1, (int)conect);
	digitalWrite(StatusLED2, (int)STLED);
	PS3Update();
	delay(20);
}

bool PS3Controller()
{
	bool b = false;
	PS3Debug = "";
	/*Vx=Vy=Angular=(double)0.0;*/
	for (int i = 0; i < 16; i++)
	{
		switch ((int)PS3button & 1ull << i) //0b/select/start/l3/r3/l1/r1/l2/r2/up/down/right/left/△/〇/×/□/
		{
		case 0b1000000000000000:
			St.addprintf(&PS3Debug, "select\n");
			break;
		case 0b0100000000000000:
			St.addprintf(&PS3Debug, "start\n");
			break;
		case 0b0010000000000000:
			St.addprintf(&PS3Debug, "l3\n");
			b = true;
			break;
		case 0b0001000000000000:
			St.addprintf(&PS3Debug, "r3\n");
			break;
		case 0b0000100000000000:
			St.addprintf(&PS3Debug, "l1\n");
			Angular = -0.5;
			break;
		case 0b0000010000000000:
			St.addprintf(&PS3Debug, "r1\n");
			Angular = 0.5;
			break;
		case 0b0000001000000000:
			St.addprintf(&PS3Debug, "l2\n");
			break;
		case 0b0000000100000000:
			St.addprintf(&PS3Debug, "r2\n");
			break;
		case 0b0000000010000000:
			St.addprintf(&PS3Debug, "up\n");
			Vy = 2.0;
			break;
		case 0b0000000001000000:
			St.addprintf(&PS3Debug, "down\n");
			Vy = -2.0;
			break;
		case 0b0000000000100000:
			St.addprintf(&PS3Debug, "right\n");
			Vx = 2.0;
			break;
		case 0b0000000000010000:
			St.addprintf(&PS3Debug, "left\n");
			Vx = -2.0;
			break;
		case 0b0000000000001000:
			St.addprintf(&PS3Debug, "triangle\n");
			break;
		case 0b0000000000000100:
			St.addprintf(&PS3Debug, "circle\n");
			break;
		case 0b0000000000000010:
			St.addprintf(&PS3Debug, "cross\n");
			break;
		case 0b0000000000000001:
			St.addprintf(&PS3Debug, "square\n");
			break;
		}
	}
	if (!PS3button)
	{
		Vx = (double(lx) / 128.0)*2.0;
		Vy = (double(ly) / 128.0) * -2.0;
		Angular = double(rx) / 128.0;
	}
	if (b)
	{
		lx = ly = rx = ry = 0;
		Vx = Vy = Angular = 0.0;
	}
	return b;
}