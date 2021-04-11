#include "SetUp.h"
#include <pcf8563.h>
#include <TFT_eSPI.h> // Graphics and font library for ST7735 driver chip
#include <SPI.h>
#include <Wire.h>
#include <WiFi.h>
#include <time.h>
#include "ttgo.h"
#include "charge.h"
#include "weather.h"
#include "compass.h"
#include "clock.h"
#include "wifirssi.h"
#include "lowpassfilter.h"
#include "MPU9250.h"
//  git clone -b development https://github.com/tzapu/WiFiManager.git
#include <WiFiManager.h> //https://github.com/tzapu/WiFiManager
#ifdef ARDUINO_OTA_UPDATE
#include <ESPmDNS.h>
#include <WiFiUdp.h>
#include <ArduinoOTA.h>
#endif

/*Instance of class*/
TFT_eSPI tft = TFT_eSPI(); // Invoke library, pins defined in User_Setup.h
PCF8563_Class rtc;
WiFiManager wifiManager;
/*Variable*/
char buff[256];
//Flag
bool rtcIrq = false;
bool initial = 1;
bool wifisetup = false;
bool pslmode = false;

uint8_t func_select = 0;
uint8_t omm = 99;
uint8_t xcolon = 0;
uint32_t targetTime = 0; // for next 1 second timeout
uint32_t colour = 0;

bool pressed = false;
uint32_t pressedTime = 0;
bool charge_indication = false;
bool wifiscansetup = false;
bool wifiscantask = false;

LowpassFilter ARoll(0.1, 0.01);
LowpassFilter APitch(0.1, 0.01);
LowpassFilter AYaw(0.1, 0.01);

float Aroll;
float Apitch;
float Ayaw;

MPU9250 IMU;

uint8_t hh, mm, ss;
/*Function*/
void wifi_scan()
{
	if (!wifiscansetup)
	{
		tft.setTextColor(TFT_GREEN, TFT_BLACK);
		tft.setTextDatum(MC_DATUM);
		tft.setTextSize(1);
		WiFi.mode(WIFI_STA);
		WiFi.disconnect();
		wifiscansetup = true;
	}
	if (t[3].stand_by(1.0))
	{
		int16_t n = WiFi.scanNetworks();
		tft.fillScreen(TFT_BLACK);
		tft.drawString("Scan Network", 0, 0);
		if (n == 0)
		{
			tft.drawString("no networks found", tft.width() / 2, tft.height() / 2);
		}
		else
		{
			tft.setTextDatum(TL_DATUM);
			tft.setCursor(0, 0);
			for (int i = 0; i < n; ++i)
			{
				sprintf(buff, "[%d]:%s(%d)", i + 1, WiFi.SSID(i).c_str(), WiFi.RSSI(i));
				Serial.println(buff);
				tft.drawString(buff, 0, 16 + i * 16);
			}
		}
	}
	//WiFi.mode(WIFI_OFF);
}
//Setup Function
void configModeCallback(WiFiManager *myWiFiManager)
{
	Serial.println("Entered config mode");
	Serial.println(WiFi.softAPIP());
	//if you used auto generated SSID, print it
	Serial.println(myWiFiManager->getConfigPortalSSID());
	myWiFiManager->setTimeout(30);
	wifisetup = true;
}
void drawProgressBar(uint16_t x0, uint16_t y0, uint16_t w, uint16_t h, uint8_t percentage, uint16_t frameColor, uint16_t barColor) //OTA
{
	if (percentage == 0)
	{
		tft.fillRoundRect(x0, y0, w, h, 3, TFT_BLACK);
	}
	uint8_t margin = 2;
	uint16_t barHeight = h - 2 * margin;
	uint16_t barWidth = w - 2 * margin;
	tft.drawRoundRect(x0, y0, w, h, 3, frameColor);
	tft.fillRect(x0 + margin, y0 + margin, barWidth * percentage / 100.0, barHeight, barColor);
}
void setupWiFi()
{
#ifdef ARDUINO_OTA_UPDATE
	WiFiManager wifiManager;
	//set callback that gets called when connecting to previous WiFi fails, and enters Access Point mode
	wifiManager.setAPCallback(configModeCallback);
	WiFi.softAPConfig(ip, ip, subnet);	   // IPアドレス、ゲートウェイ、サブネットマスクの設定
	wifiManager.setBreakAfterConfig(true); // Without this saveConfigCallback does not get fired
	wifiManager.setAPClientCheck(true);
	wifiManager.autoConnect("T-Wristband");
#endif
}
void setupOTA()
{
#ifdef ARDUINO_OTA_UPDATE
	// Port defaults to 3232
	// ArduinoOTA.setPort(3232);
	// Hostname defaults to esp3232-[MAC]
	ArduinoOTA.setHostname("T-Wristband");
	// No authentication by default
	// ArduinoOTA.setPassword("admin");
	// Password can be set with it's md5 value as well
	// MD5(admin) = 21232f297a57a5a743894a0e4a801fc3
	// ArduinoOTA.setPasswordHash("21232f297a57a5a743894a0e4a801fc3");
	ArduinoOTA.onStart([]() {
				  String type;
				  if (ArduinoOTA.getCommand() == U_FLASH)
					  type = "sketch";
				  else // U_SPIFFS
					  type = "filesystem";

				  // NOTE: if updating SPIFFS this would be the place to unmount SPIFFS using SPIFFS.end()
				  digitalWrite(LED_PIN, LOW);
				  Serial.println("Start updating " + type);
				  otaStart = true;
				  tft.fillScreen(TFT_BLACK);
				  tft.setTextColor(TFT_GREEN, TFT_BLACK);
				  tft.drawString("Updating...", tft.width() / 2 - 20, 55);
			  })
		.onEnd([]() {
			Serial.println("\nEnd");
			delay(500);
		})
		.onProgress([](unsigned int progress, unsigned int total) {
			// Serial.printf("Progress: %u%%\r", (progress / (total / 100)));
			int percentage = (progress / (total / 100));
			tft.setTextDatum(TC_DATUM);
			tft.setTextPadding(tft.textWidth(" 888% "));
			tft.drawString(String(percentage) + "%", 145, 35);
			drawProgressBar(10, 30, 120, 15, percentage, TFT_WHITE, TFT_BLUE);
			if (t[3].stand_by(1.0))
				digitalWrite(LED_PIN, HIGH);
			else
				digitalWrite(LED_PIN, LOW);
		})
		.onError([](ota_error_t error) {
			Serial.printf("Error[%u]: ", error);
			if (error == OTA_AUTH_ERROR)
				Serial.println("Auth Failed");
			else if (error == OTA_BEGIN_ERROR)
				Serial.println("Begin Failed");
			else if (error == OTA_CONNECT_ERROR)
				Serial.println("Connect Failed");
			else if (error == OTA_RECEIVE_ERROR)
				Serial.println("Receive Failed");
			else if (error == OTA_END_ERROR)
				Serial.println("End Failed");

			tft.fillScreen(TFT_BLACK);
			tft.drawString("Update Failed", tft.width() / 2 - 20, 55);
			delay(3000);
			otaStart = false;
			initial = 1;
			targetTime = millis() + 1000;
			tft.fillScreen(TFT_BLACK);
			tft.setTextDatum(TL_DATUM);
			omm = 99;
		});

	ArduinoOTA.begin();
#endif
}
void setuptime() //Wifi time setting
{
	//init and get the time
	configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);
	struct tm timeinfo;
	if (!getLocalTime(&timeinfo))
		Serial.println("Failed to obtain time");
	else
	{
		rtc.setDateTime(timeinfo.tm_year + 1900, timeinfo.tm_mon + 1, timeinfo.tm_mday, timeinfo.tm_hour, timeinfo.tm_min, timeinfo.tm_sec);
		Serial.println(&timeinfo, "Set %Y %m %d %a %H:%M:%S");
		RTC_Date datetime = rtc.getDateTime();
		hh = datetime.hour;
		mm = datetime.minute;
		ss = datetime.second;
		Serial.println("hh=" + String(hh) + "mm=" + String(mm) + "ss=" + String(ss));
	}
}
void setupRTC()
{
	rtc.begin(Wire);
	//Check if the RTC clock matches, if not, use compile time
	rtc.check();
	RTC_Date datetime = rtc.getDateTime();
	hh = datetime.hour;
	mm = datetime.minute;
	ss = datetime.second;
	Serial.println("hh=" + String(hh) + "mm=" + String(mm) + "ss=" + String(ss));
}
void setup(void)
{
	Serial.begin(115200);
	pinMode(TP_PIN_PIN, INPUT);
	pinMode(TP_PWR_PIN, OUTPUT);
	digitalWrite(TP_PWR_PIN, HIGH);
	pinMode(LED_PIN, OUTPUT);
	pinMode(CHARGE_PIN, INPUT_PULLUP);
	attachInterrupt(
		CHARGE_PIN, [] { charge_indication = true; }, CHANGE);
	if (digitalRead(CHARGE_PIN) == LOW)
		charge_indication = true;
	tft.init();
	tft.setRotation(1);
	tft.setSwapBytes(true);
	tft.pushImage(0, 0, 160, 80, ttgo);
	Wire.begin(I2C_SDA_PIN, I2C_SCL_PIN);
	Wire.setClock(400000);
	IMU.setup();
	setupADC();
	setupRTC();
	tft.fillScreen(TFT_BLACK);
	tft.setTextColor(TFT_YELLOW, TFT_BLACK); // Note: the new fonts do not draw the background colour
	targetTime = millis() + 1000;
	for (int i = 0; i < Timernum; i++)
		t[i].reset();
	sleeptime = 30;
	wifiscansetup = false;
	wifiscantask = false;
	xTaskCreatePinnedToCore(wifitask, "wifitask", 8096, NULL, 1, NULL, 1);
}

//Main Function
void wifitask(void *arg)
{
	float level = BTLEVEL();
	if (level > PSMLEVEL)
	{
		setupWiFi();
		setupOTA();
		setuptime();
		xTaskCreatePinnedToCore(weathertask, "weathertask", 2024, NULL, 1, NULL, 1);
		if (wifisetup)
		{
			func_select = funcnum;
			tft.fillScreen(TFT_BLACK);
		}
	}
	else
		pslmode = true;
	vTaskDelete(NULL);
	wifisetup = false;
}

void weather_show()
{
	if (t[3].stand_by(0.5))
	{
		static uint8_t t = 0;
		tft.setTextColor(TFT_GREEN, TFT_BLACK);
		tft.setTextDatum(TL_DATUM);
		snprintf(buff, sizeof(buff), "Tokyo weather");
		tft.drawString(buff, 0, 0);
		if (!httperror)
		{
			tft.drawString("Today:" + String(now_weather) + " " + String(now_temp) + "[C]   ", 0, 16);
			if (now_weather == "Clear")
				drawmark(&tft, 0, t, TFT_YELLOW, 140, 16);
			else if (now_weather == "Clouds")
				drawmark(&tft, 1, t, TFT_DARKGREY, 140, 16);
			else if (now_weather == "Rain")
				drawmark(&tft, 2, t, TFT_BLUE, 140, 16);
			else if (now_weather == "Mist")
				drawmark(&tft, 3, t, TFT_SILVER, 140, 16);
			else if (tom_weather == "Snow")
				drawmark(&tft, 4, t, TFT_WHITE, 140, 16);
			tft.drawString("tomorrow:" + String(tom_weather) + " " + String(tom_temp) + "[C]   ", 0, 32);
			if (tom_weather == "Clear")
				drawmark(&tft, 0, t, TFT_YELLOW, 140, 32);
			else if (tom_weather == "Clouds")
				drawmark(&tft, 1, t, TFT_DARKGREY, 140, 32);
			else if (tom_weather == "Rain")
				drawmark(&tft, 2, t, TFT_BLUE, 140, 32);
			else if (tom_weather == "Mist")
				drawmark(&tft, 3, t, TFT_SILVER, 140, 32);
			else if (tom_weather == "Snow")
				drawmark(&tft, 4, t, TFT_WHITE, 140, 32);
			t++;
			if (t > 3)
				t = 0;
		}
		else
		{
			snprintf(buff, sizeof(buff), "Error on HTTP request");
			tft.drawString(buff, 0, 16);
		}
	}
	//delay(500);
}

void RTC_Show()
{
	if (targetTime < millis())
	{
		RTC_Date datetime = rtc.getDateTime();
		hh = datetime.hour;
		mm = datetime.minute;
		ss = datetime.second;
		//Serial.printf("hh:%d mm:%d ss:%d\n", hh, mm, ss);
		targetTime = millis() + 1000;
		if (ss == 0 || initial)
		{
			initial = 0;
			tft.setCursor(8, 50);
			tft.print(Week(&tft, Weeknum(datetime.month, datetime.day, datetime.year)));
			tft.setTextColor(TFT_GREENYELLOW, TFT_BLACK);
			tft.setCursor(8, 60);
			//tft.print(__DATE__); // This uses the standard ADAFruit small font
			tft.print(DATE(datetime.month, datetime.day, datetime.year));
		}
		// Update digital time
		uint8_t xpos = 6;
		uint8_t ypos = 0;
		if (omm != mm)
		{ // Only redraw every minute to minimise flicker
			// Uncomment ONE of the next 2 lines, using the ghost image demonstrates text overlay as time is drawn over it
			tft.setTextColor(0x39C4, TFT_BLACK); // Leave a 7 segment ghost image, comment out next line!
			//tft.setTextColor(TFT_BLACK, TFT_BLACK); // Set font colour to black to wipe image
			// Font 7 is to show a pseudo 7 segment display.
			// Font 7 only contains characters [space] 0 1 2 3 4 5 6 7 8 9 0 : .
			tft.drawString("88:88", xpos, ypos, 7); // Overwrite the text to clear it
			tft.setTextColor(0xFBE0, TFT_BLACK);	// Orange
			omm = mm;

			if (hh < 10)
				xpos += tft.drawChar('0', xpos, ypos, 7);
			xpos += tft.drawNumber(hh, xpos, ypos, 7);
			xcolon = xpos;
			xpos += tft.drawChar(':', xpos, ypos, 7);
			if (mm < 10)
				xpos += tft.drawChar('0', xpos, ypos, 7);
			tft.drawNumber(mm, xpos, ypos, 7);
		}

		if (ss % 2)
		{ // Flash the colon
			tft.setTextColor(0x39C4, TFT_BLACK);
			xpos += tft.drawChar(':', xcolon, ypos, 7);
			tft.setTextColor(0xFBE0, TFT_BLACK);
		}
		else
		{
			tft.setTextColor(TFT_BLUE, TFT_BLACK);
			tft.drawChar(':', xcolon, ypos, 7);
		}
		//weather
		const uint8_t x = 0;
		const uint8_t y = 72;
		const uint8_t mx = 75;
		static uint8_t t = 0;
		if (!httperror && !wifisetup && !pslmode)
		{
			int color = TFT_BLACK;
			if (now_weather == "Clear")
			{
				color = TFT_YELLOW;
				drawmark(&tft, 0, t, TFT_YELLOW, x + mx, y);
			}
			else if (now_weather == "Clouds")
			{
				color = TFT_DARKGREY;
				drawmark(&tft, 1, t, TFT_DARKGREY, x + mx, y);
			}
			else if (now_weather == "Rain")
			{
				color = TFT_BLUE;
				drawmark(&tft, 2, t, TFT_BLUE, x + mx, y);
			}
			else if (now_weather == "Mist")
			{
				color = TFT_SILVER;
				drawmark(&tft, 3, t, TFT_SILVER, x + mx, y);
			}
			else if (tom_weather == "Snow")
			{
				color = TFT_WHITE;
				drawmark(&tft, 4, t, TFT_WHITE, x + mx, y);
			}
			tft.setTextColor(color, TFT_BLACK);
			tft.drawString("Today:" + String(now_weather), x, y);
			t++;
			if (t > 3)
				t = 0;
		}
		else
		{
			if (!pslmode)
			{
				tft.setTextColor(TFT_RED, TFT_BLACK);
				tft.drawString("Offline", x, y);
			}
			else
			{
				tft.setTextColor(TFT_SKYBLUE, TFT_BLACK);
				tft.drawString("Power saving", x, y);
			}
		}
	}
	BTlevel(&tft, 85, 60);
	drawwifi(&tft, TFT_WHITE, TFT_SILVER, 140, 60);
}

void IMU_Show()
{
	if (t[3].stand_by(0.2))
	{
		tft.setTextColor(TFT_GREEN, TFT_BLACK);
		tft.fillScreen(TFT_BLACK);
		tft.setTextDatum(TL_DATUM);
		IMU.update();
		float ax = IMU.read(AccX);
		float ay = IMU.read(AccY);
		float az = IMU.read(AccZ);
		float gx = IMU.read(GyroX);
		float gy = IMU.read(GyroY);
		float gz = IMU.read(GyroZ);
		float mx = IMU.read(MagX);
		float my = IMU.read(MagY);
		float mz = IMU.read(MagZ);
		float temp = IMU.read(Temp);
		snprintf(buff, sizeof(buff), "--  ACC  GYR   MAG %.1f[C]", temp);
		tft.drawString(buff, 0, 0);
		snprintf(buff, sizeof(buff), "x %6.2f  %6.2f  %6.2f", ax, gx, mx);
		tft.drawString(buff, 0, 16);
		snprintf(buff, sizeof(buff), "y %6.2f  %6.2f  %6.2f", ay, gy, my);
		tft.drawString(buff, 0, 32);
		snprintf(buff, sizeof(buff), "z %6.2f  %6.2f  %6.2f", az, gz, mz);
		tft.drawString(buff, 0, 48);
	}
}

void calendar()
{
	const int lcolor = TFT_WHITE;
	if (t[3].stand_by(0.2))
	{
		tft.setTextColor(TFT_YELLOW, TFT_BLACK);
		tft.setTextDatum(TL_DATUM);
		RTC_Date datetime = rtc.getDateTime();
		hh = datetime.hour;
		mm = datetime.minute;
		ss = datetime.second;
		uint8_t Nowweek = Weeknum(datetime.month, datetime.day, datetime.year);
		tft.drawString(String(datetime.year), 0, 0);
		if (IsLeapYear(datetime.year))
			tft.drawString("leap year", 60, 0);
		tft.setTextColor(TFT_PINK, TFT_BLACK);
		tft.drawString(DATE(datetime.month, datetime.day, datetime.year, true), 30, 0);
		tft.drawLine(0, 10, 210, 10, lcolor);
		tft.drawLine(0, 20, 210, 20, lcolor);
		tft.drawLine(0, 32, 210, 32, lcolor);
		int lastday = GetLastDay(datetime.year, datetime.month);
		int plastday = GetLastDay(datetime.year, (datetime.month != 1) ? datetime.month - 1 : 12);

		for (int i = 0; i < 7; i++)
		{
			int x = 23 * i + 2;
			tft.drawString(Week(&tft, i, true, false), x, 12);
			if (i == Nowweek)
				tft.setTextColor(TFT_GOLD, TFT_NAVY);
			else if (i == 0)
				tft.setTextColor(TFT_SKYBLUE, TFT_BLACK);
			else if (i == 6)
				tft.setTextColor(TFT_SKYBLUE, TFT_BLACK);
			else
				tft.setTextColor(lcolor, TFT_BLACK);
			int day = datetime.day - Nowweek + i;
			if (day > lastday)
			{
				day -= lastday;
				tft.setTextColor(TFT_LIGHTGREY, TFT_BLACK);
			}
			else if (day < 1)
			{
				day = plastday - abs(day);
				tft.setTextColor(TFT_LIGHTGREY, TFT_BLACK);
			}
			snprintf(buff, sizeof(buff), " %0d", day);
			tft.drawString(buff, x, 22);
			int range = (i != 0) ? 3 : 2;
			tft.drawLine(x - range, 10, x - range, 32, lcolor);
		}
		tft.fillRoundRect(0, 35, 159, 14, 0, TFT_BLUE);
		static uint8_t t = 0;
		static int16_t x;
		x = map(datetime.hour, 0, 24, 0, 160);
		drawmark(&tft, 0, t, TFT_YELLOW, x, 38, TFT_BLUE);
		t++;
		if (t > 3)
			t = 0;

		tft.drawLine(159, 10, 159, 32, lcolor);
		tft.setTextColor(TFT_ORANGE, TFT_BLACK);
		snprintf(buff, sizeof(buff), " %02d:%02d", hh, mm);
		tft.drawString(buff, 0, 60, 2);

		tft.setTextColor(TFT_BLUE, TFT_BLACK);
		tft.drawString("0", 0, 50);
		tft.setTextColor(TFT_YELLOW, TFT_BLACK);
		tft.drawString("12", 75, 50);
		tft.setTextColor(TFT_BLUE, TFT_BLACK);
		tft.drawString("23", 148, 50);
	}
	BTlevel(&tft, 85, 60);
	drawwifi(&tft, TFT_WHITE, TFT_SILVER, 140, 60);
}

void printLocalTime()
{
	struct tm timeinfo;
	if (t[3].stand_by(0.2))
	{
		tft.setTextColor(TFT_GREEN, TFT_BLACK);
		tft.setTextDatum(TL_DATUM);
		snprintf(buff, sizeof(buff), "Status");
		tft.drawString(buff, 0, 0);
		if (!getLocalTime(&timeinfo))
		{
			snprintf(buff, sizeof(buff), "Failed to obtain time");
			tft.drawString(buff, 0, 16);
			Serial.println("Failed to obtain time");
			return;
		}
		setuptime();
		tft.setTextColor(TFT_RED, TFT_BLACK);
		snprintf(buff, sizeof(buff), "Get Time:success!");
		tft.drawString(buff, 0, 16);
		tft.setTextColor(TFT_BLUE, TFT_BLACK);
		snprintf(buff, sizeof(buff), " %04d/%02d/%02d %02d:%02d:%02d", timeinfo.tm_year + 1900, timeinfo.tm_mon + 1, timeinfo.tm_mday, timeinfo.tm_hour, timeinfo.tm_min, timeinfo.tm_sec);
		Serial.println(&timeinfo, "%Y %m %d %a %H:%M:%S"); //日本人にわかりやすい表記へ変更
		tft.drawString(buff, 0, 32);
		tft.setTextColor(TFT_MAGENTA, TFT_BLACK);
		snprintf(buff, sizeof(buff), "CPU:%.1f[C]", CPUTemp());
		tft.drawString(buff, 80, 48);
		drawwifi(&tft, TFT_WHITE, TFT_SILVER, 60, 60);
		tft.setTextColor(TFT_SKYBLUE, TFT_BLACK);
		snprintf(buff, sizeof(buff), "WIFI:%ld[dBm]", WiFi.RSSI());
		tft.drawString(buff, 80, 64);
	}
	BTlevel(&tft, 0, 50);
}

void clocktask()
{
	if (t[3].stand_by(0.2))
	{
		RTC_Date datetime = rtc.getDateTime();
		hh = datetime.hour;
		mm = datetime.minute;
		ss = datetime.second;
		tft.drawString(Week(&tft, Weeknum(datetime.month, datetime.day, datetime.year)), 90, 7);
		tft.setTextColor(TFT_GOLD, TFT_BLACK);
		snprintf(buff, sizeof(buff), "%d/%d", datetime.month, datetime.day);
		tft.drawString(buff, 90, 17);
	}
	drawwifi(&tft, TFT_WHITE, TFT_SILVER, 80, 50);
	BTlevel(&tft, 100, 50, 0.5f);
	if (drawCLOCK(&tft, 40, 40, TFT_DARKGREY, TFT_SILVER, TFT_WHITE, TFT_MAROON, hh, mm, ss, TFT_NAVY))
	{
		tft.setTextDatum(TL_DATUM);
		tft.setTextColor(TFT_BROWN, TFT_BLACK);
		snprintf(buff, sizeof(buff), " %02d:%02d", hh, mm);
		tft.drawString(buff, 90, 26, 2);
		tft.setTextColor(TFT_MAROON, TFT_BLACK);
		snprintf(buff, sizeof(buff), " %02d", ss);
		tft.drawString(buff, 130, 32);
	}
}

Chart a1, a2, a3;
void Charttask()
{
	if (t[3].stand_by(0.1))
	{
		tft.setTextColor(TFT_GREEN, TFT_BLACK);
		tft.setTextDatum(TL_DATUM);
		IMU.update();
		float ax = IMU.read(AccX);
		float ay = IMU.read(AccY);
		float az = IMU.read(AccZ);
		float gx = IMU.read(GyroX);
		float gy = IMU.read(GyroY);
		float gz = IMU.read(GyroZ);
		float mx = IMU.read(MagX);
		float my = IMU.read(MagY);
		float mz = IMU.read(MagZ);
		float temp = IMU.read(Temp);
		a1.Chartprint(&tft, 10 * ax, tft.height() / 2 + 25, 0, TFT_RED);
		a2.Chartprint(&tft, 10 * ay, tft.height() / 2, 0, TFT_BLUE);
		a3.Chartprint(&tft, 10 * az, tft.height() / 2 - 25, 0, TFT_YELLOW);
	}
}

void comp()
{
	if (t[3].stand_by(0.2))
	{
		tft.setTextColor(TFT_GREEN, TFT_BLACK);
		tft.fillScreen(TFT_BLACK);
		tft.setTextDatum(TL_DATUM);

		IMU.update();
		float ax = IMU.read(AccX);
		float ay = IMU.read(AccY);
		float az = IMU.read(AccZ);
		float gx = IMU.read(GyroX);
		float gy = IMU.read(GyroY);
		float gz = IMU.read(GyroZ);
		float mx = IMU.read(MagX);
		float my = IMU.read(MagY);
		float mz = IMU.read(MagZ);
		float temp = IMU.read(Temp);

		//加速度から傾斜（roll,pitch）を算出
		Aroll = atan2f(ay, az);
		Apitch = -atan2f(ax, sqrtf(ay * ay + az * az));

		//地磁気から回転角（yaw）を算出
		float Mx = mx * cosf(Apitch) + my * sinf(Aroll) * sinf(Apitch) + mz * cosf(Aroll) * sinf(Apitch);
		float My = my * cosf(Aroll) - mz * sinf(Aroll);
		Ayaw = atan2f(My, Mx);

		//加速度と地磁気から求めた角度をローパスフィルターに通す
		ARoll = Aroll * RAD_TO_DEG;
		APitch = Apitch * RAD_TO_DEG;
		AYaw = Ayaw * RAD_TO_DEG;

		float b = (float)AYaw;

		float Bearing = (b < 0.f) ? 360 + b : b;
		const char *s = "N";
		if (337.5 < Bearing || Bearing < 22.5)
			s = "N";
		else if (22.5 < Bearing && Bearing < 67.5)
			s = "NE";
		else if (67.5 < Bearing && Bearing < 112.5)
			s = "E";
		else if (112.5 < Bearing && Bearing < 157.5)
			s = "SE";
		else if (157.5 < Bearing && Bearing < 202.5)
			s = "S";
		else if (202.5 < Bearing && Bearing < 247.5)
			s = "SW";
		else if (247.5 < Bearing && Bearing < 292.5)
			s = "W";
		else if (292.5 < Bearing && Bearing < 337.5)
			s = "NW";
		snprintf(buff, sizeof(buff), "%s %.2f[deg]", s, Bearing);
		drawcomp(&tft, 40, 40, TFT_MAGENTA, Bearing);
		tft.drawString(buff, 0, 0);
		snprintf(buff, sizeof(buff), "MX=%.2f", Mx);
		tft.drawString(buff, 100, 32);
		snprintf(buff, sizeof(buff), "MY=%.2f", My);
		tft.drawString(buff, 100, 48);
	}
}

void wifitask()
{
	if (t[3].stand_by(0.5))
	{
		tft.fillScreen(TFT_BLACK);
		tft.setTextColor(TFT_WHITE);
		tft.drawString("Connect hotspot name ", 20, tft.height() / 2 - 20);
		tft.drawString("configure wrist", 35, tft.height() / 2 + 20);
		tft.setTextColor(TFT_GREEN);
		tft.drawString("\"T-Wristband\"", 40, tft.height() / 2);
	}
}

void loop()
{
	static bool Pflag = false, cflag = false;
	static int count = 0;
#ifdef ARDUINO_OTA_UPDATE
	if (!pslmode)
		ArduinoOTA.handle();
#endif
	//! If OTA starts, skip the following operation
	if (!otaStart)
	{
		float level = BTLEVEL();
		if (level <= 10.0f && !pslmode)
		{
			WiFi.disconnect();
			WiFi.mode(WIFI_OFF);
			pslmode = true;
		}
		if (charge_indication)
		{
			charge_indication = false;
		}
		if (digitalRead(TP_PIN_PIN) == HIGH)
		{
			if (!pressed)
			{
				t[0].reset();
				initial = 1;
				targetTime = millis() + 1000;
				omm = 99;
				digitalWrite(LED_PIN, HIGH);
				pressed = true;
				Pflag = true;
				count++;
				pressedTime = millis();
			}
			else
			{
				if (millis() - pressedTime > 1500)
				{
					if (count <= 1 && func_select != 5 && func_select != (funcnum - 1))
						func_select = funcnum;
					else if (func_select == (funcnum - 1))
						wifiscantask = cflag = true;
					else
					{
						if (!cflag)
						{
							CO.inistart = !CO.inistart;
							CO.reset();
							cflag = true;
						}
					}
				}
				if (millis() - pressedTime > 4000)
				{
					if (count >= 2)
					{
						digitalWrite(LED_PIN, LOW);
						tft.fillScreen(TFT_BLACK);
						tft.setTextColor(TFT_YELLOW, TFT_BLACK);
						tft.drawString("Reset WiFi Setting", 20, tft.height() / 2);
						delay(3000);
						wifiManager.resetSettings();
						wifiManager.erase(true);
						esp_restart();
					}
				}
			}
		}
		else
		{
			if (t[4].stand_by(0.30))
			{
				if (Pflag)
				{
					if (!cflag)
						func_select = (func_select + 1 >= funcnum - 1) ? 0 : func_select + 1;
					tft.fillScreen(TFT_BLACK);
					cflag = Pflag = false;
				}
				digitalWrite(LED_PIN, LOW);
				count = 0;
			}
			if (t[0].stand_by(sleeptime) && wifisetup == false)
				func_select = funcnum;
			pressed = false;
		}

		switch (func_select)
		{
		case 0:
			RTC_Show();
			break;
		case 1:
			clocktask();
			break;
		case 2:
			calendar();
			break;
		case 3:
			IMU_Show();
			break;
		case 4:
			Charttask();
			break;
		case 5:
			comp();
			break;
		case 6:
			if (!wifisetup && !pslmode)
			{
				printLocalTime();
				break;
			}
		case 7:
			if (!wifisetup && !pslmode)
			{
				weather_show();
				break;
			}
		case funcnum - 1:
			if (wifisetup && !pslmode)
			{
				func_select = funcnum - 1;
				if (!wifiscantask)
					wifitask();
				else
					wifi_scan();
			}
			else if (pslmode)
				func_select = 0;
			break;
		case funcnum:
			digitalWrite(LED_PIN, LOW);
			t[0].reset();
			if (!wifisetup && !pslmode)
			{
				setuptime();
			}
			tft.fillScreen(TFT_BLACK);
			tft.setTextColor(TFT_GREEN, TFT_BLACK);
			tft.setTextDatum(MC_DATUM);
			tft.drawString("Press again to wake up", tft.width() / 2, tft.height() / 2);
			IMU.setSleepEnabled(true);
			Serial.println("Go to Sleep");
			tft.setTextColor(TFT_SILVER, TFT_BLACK);
			delay(800);
			tft.drawString("z", (tft.width() / 2), (tft.height() / 2) + 20);
			delay(800);
			tft.drawString("zz", (tft.width() / 2), (tft.height() / 2) + 20);
			delay(800);
			tft.drawString("zzz", (tft.width() / 2), (tft.height() / 2) + 20);
			delay(600);
			//delay(3000);
			pinMode(TP_PWR_PIN, PULLUP); //! Must be set to pull-up output mode in order to wake up in deep sleep mode
			tft.writecommand(ST7735_SLPIN);
			tft.writecommand(ST7735_DISPOFF);
			esp_sleep_enable_ext1_wakeup(GPIO_SEL_33, ESP_EXT1_WAKEUP_ANY_HIGH);
			esp_deep_sleep_start();
			break;
		default:
			break;
		}
	}
	delay(1);
}
