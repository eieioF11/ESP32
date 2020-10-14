#ifndef __ROBO_MOTHER_MINI_H__
#define __ROBO_MOTHER_MINI_H__
#include <Arduino.h>
#include <WiFi.h>
#include <ArduinoOTA.h>
#include <ESPmDNS.h>
#include "mcp_can.h"
#include "remote_switch.h"

#define router_ssid    "biz"
#define router_pw      "roborobo"

#define ap_ssid         "roboBauto"
#define ap_pw           "roborobo"

#define LocalPort 10000
//const IPAddress ip(192,168,30,3);
//const IPAddress subnet(255,255,255,0);

#define e_ON 1
#define e_OFF 0
extern bool em;

void OTA_Setup();
void e_stop(uint8_t sw);//emergency_stop
void CAN_Init();

extern MCP_CAN CAN0;

//ChipSelects
#define DEV1    GPIO_NUM_13
#define DEV2    GPIO_NUM_14
#define DEV3    GPIO_NUM_12
#define DEV4    GPIO_NUM_27
#define DEV5    GPIO_NUM_4      //Top Socket No.1
#define DEV6    GPIO_NUM_16     //Top Socket No.2
#define DEV7    GPIO_NUM_17     //Top Socket No.3
#define DEV8    GPIO_NUM_15     //Top Socket No.4
#define DEV9    GPIO_NUM_18//sol1
#define DEV10   GPIO_NUM_19//sol2
#define CS_CAN  GPIO_NUM_26
const gpio_num_t DEV[10]{
  DEV1,DEV2,DEV3,DEV4,DEV5,
  DEV6,DEV7,DEV8,DEV9,DEV10
};
//SPI port
#define MOSI    GPIO_NUM_32
#define MISO    GPIO_NUM_33
#define SCLK    GPIO_NUM_25

//I2C port
#define SCL     GPIO_NUM_22
#define SDA     GPIO_NUM_21

//INPUT
#define START_SWITCH    GPIO_NUM_23
#define CAN_INT         GPIO_NUM_34

//OUTPUT
#define POWER_CONTROL   GPIO_NUM_2
#define SOUNDER         GPIO_NUM_5

//音を鳴らす時間
#define BEAT 500
//音階名と周波数の対応
#define C4 261.6
#define D4 293.665
#define E4 329.63
#define F4 349.228
#define G4 391.995
#define A4 440
#define B4 493.883
#define C5 523.251

#endif