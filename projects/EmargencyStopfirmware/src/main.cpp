#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <WiFiUDP.h>

// UDP settings
WiFiUDP udp;

// WiFi settings
const IPAddress sendip(192,168,30,4);//送り先のIP
const IPAddress IP(192, 168, 30, 6);
const char *ssid = "ESP32ROBO";
const char *password = "roborobo";
#define LocalPort 10000 //local host

#define LED1 14
#define BTLED 13
#define EMSPIN 12

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
	digitalWrite(LED1,HIGH);
}

void setup()
{
	Serial.begin(115200);
	pinMode(EMSPIN, INPUT);
	pinMode(LED1, OUTPUT);
	pinMode(BTLED, OUTPUT);
	Serial.println("start");
	WiFi.mode(WIFI_STA); //重要!
	udp.begin(LocalPort);
	digitalWrite(LED1,LOW);
	digitalWrite(BTLED,LOW);
	connectWiFi();
}

bool flag=false;
int count=0;
void loop()
{
	EMS=EMSW();
	if(EMS)
	{
		count=0;
		flag=false;
		udp.beginPacket(sendip, LocalPort);
		udp.write(1);
		udp.endPacket();
		Serial.printf("EMS send\n\r");
		digitalWrite(BTLED,LOW);
	}
	else
	{
		digitalWrite(BTLED,HIGH);
		if(!flag)
		{
			udp.beginPacket(sendip, LocalPort);
			udp.write(0);
			udp.endPacket();
			Serial.printf("0 send\n\r");
			if(count>20)
			{
				count=0;
				flag=true;
			}
			else
				count++;
		}
	}
	if (WiFi.status() != WL_CONNECTED)
	{
		digitalWrite(LED1,LOW);
		WiFi.disconnect();
		Serial.println("disconnect!");
		connectWiFi();
	}
	delay(10);
}