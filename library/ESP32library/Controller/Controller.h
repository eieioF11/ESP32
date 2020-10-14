#ifndef CONTROLLER_H_
#define CONTROLLER_H_
#include "Setup.h"
#include <Arduino.h>
#include "WiFi.h"
#include "Sound.h"
#include "Timer.h"

#if (ESP_OTAE == 1)
#include <ArduinoOTA.h>
#endif

#define up 32
#define down 13
#define left 33
#define right 14

#define set 16
#define stop 25

#define A 22
#define B 21
#define C 5
#define D 17
 

#if defined __cplusplus
extern "C" {
#endif

extern byte send_byte;

//使用可能なクラスのオブジェクト
extern WiFiUDP udp;

void Controllerinit();  //setup()で１回実行
void ControllerUpdate();

#if defined __cplusplus
}
#endif
#endif