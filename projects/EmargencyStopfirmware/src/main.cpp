#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <WiFiUDP.h>

// UDP settings
WiFiUDP udp;

// WiFi settings
IPAddress IP(192, 168, 30, 6);
const char *ssid = "ESP32ROBO";
const char *password = "roborobo";
#define LocalPort 10000 //local host
#define LED1 0
#define LED2 1
#define EMSPIN 3

inline int EMSW()
{
	return digitalRead(EMSPIN);
}
bool EMS;

void connectWiFi()
{
	WiFi.begin(ssid, password);
	WiFi.config(IP, WiFi.gatewayIP(), WiFi.subnetMask());

	Serial.println("start_connect");
	while (WiFi.status() != WL_CONNECTED)
	{
		delay(500);
		Serial.print(".");
	}
	Serial.println("CONNECTED!");
}

void setup()
{
	Serial.begin(115200);
	/*
	pinMode(EMSPIN, INPUT);
	pinMode(LED1, OUTPUT);
	pinMode(LED2, OUTPUT);
	*/
	Serial.println("start");
	WiFi.mode(WIFI_STA); //重要!
	udp.begin(LocalPort);
	connectWiFi();
}

void loop()
{
	EMS=EMSW();
	Serial.printf("EMS %d\n\r",EMS);
	udp.beginPacket(IP, LocalPort);
	udp.write((byte)EMS);
	udp.endPacket();
	if (WiFi.status() != WL_CONNECTED)
	{
		WiFi.disconnect();
		Serial.println("disconnect!");
		connectWiFi();
	}
}