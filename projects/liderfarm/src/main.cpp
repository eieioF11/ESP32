#include <Arduino.h>
#include "stprint.h"
#include "Wire.h"
#include "Setup.h"
#include "Lider.h"
#include "SD.h"

fptr_vv_t ESPtask_i2c_init = NULL;
fptr_vv_t ESPtask_i2c_while = []() {};

void ESPtask_i2c(void *arg)
{
	pinMode(ESP_SDA, PULLUP);
	pinMode(ESP_SCL, PULLUP);
	Wire.begin(ESP_SDA, ESP_SCL, 400000);
	if (ESPtask_i2c_init != NULL)
		ESPtask_i2c_init();
	portTickType lt = xTaskGetTickCount();
	while (1)
	{
		vTaskDelayUntil(&lt, 1 / portTICK_RATE_MS);
		ESPtask_i2c_while();
	}
}
Lider lider;
byte data[14];

void setup()
{
	Serial.begin(115200);
	ESPtask_i2c_init = []() {
		lider.setup();
	}; //i2cを使用するものの初期化はここに
	ESPtask_i2c_while = []() {
		lider.update();
		uint16_t dis = (uint16_t)lider.read(Distance);
		float angle = lider.read(Angle);
		data[0] = dis >> 8;
		data[1] = dis & 0xff;
		uint16_t Deg = (uint16_t)angle;
		data[2] = Deg >> 8;
		data[3] = Deg & 0xff;
		Serial.write(data, 4);
		//.printf("%d,%f\n\n",dis,angle);Serial.printf("%d,%f\n\n",dis,angle);
		//if(gVL530X.timeoutOccurred())//st.printf("/VL53L0XTimeOut/");
		//else st.printf("//");
	};
	xTaskCreatePinnedToCore(ESPtask_i2c, "ESPi2c", 8096, NULL, 1, NULL, 0);
}

void loop()
{
}